#include "camera_V4L2/LiveCaptureParameters.h"

#include <QDebug>

#include <fcntl.h>
#include <libv4l2.h>
#include <linux/videodev2.h>
#include <sys/mman.h>

namespace ccs::camera_V4L2
{

static v4l2_queryctrl queryCtrl(int fd, uint32_t controlId)
{
	v4l2_queryctrl r;
	memset(&r, 0, sizeof(r));
	r.id = controlId;

	if (v4l2_ioctl(fd, VIDIOC_QUERYCTRL, &r) < 0)
		qFatal("VIDIOC_QUERYCTRL failed");

	return r;
}

static int getCtrl(int fd, uint32_t controlId)
{
	v4l2_control control;
	memset(&control, 0, sizeof(control));
	control.id = controlId;

	if (v4l2_ioctl(fd, VIDIOC_G_CTRL, &control) < 0)
		return 0;
	
	return control.value;
}

static int setCtrl(int fd, uint32_t controlId, int value)
{
	v4l2_control control;
	memset(&control, 0, sizeof(control));
	control.id = controlId;
	control.value = value;

	v4l2_ioctl(fd, VIDIOC_S_CTRL, &control);
}

static QMap<int, QString> enumMenu(int fd, uint32_t controlId)
{
	v4l2_queryctrl queryctrl = queryCtrl(fd, controlId);
	v4l2_querymenu querymenu;

	memset(&querymenu, 0, sizeof(querymenu));
	querymenu.id = controlId;

	QMap<int, QString> r;
	for (querymenu.index = queryctrl.minimum;
		querymenu.index <= queryctrl.maximum;
		querymenu.index++)
	{
		if (v4l2_ioctl(fd, VIDIOC_QUERYMENU, &querymenu) == 0)
			r.insert(querymenu.index, (const char*)querymenu.name);
	}

	return r;
}

static bool canBeRead(int fd, uint32_t controlId)
{
	uint32_t flags = queryCtrl(fd, controlId).flags;

	// return true unless one of the following flags is set
	return (flags & (
		V4L2_CTRL_FLAG_INACTIVE |
		V4L2_CTRL_FLAG_WRITE_ONLY)) == 0;
}

static bool canBeWritten(int fd, uint32_t controlId)
{
	uint32_t flags = queryCtrl(fd, controlId).flags;

	// return true unless one of the following flags is set
	return (flags & (
		V4L2_CTRL_FLAG_GRABBED |
		V4L2_CTRL_FLAG_READ_ONLY |
		V4L2_CTRL_FLAG_INACTIVE |
		V4L2_CTRL_FLAG_MODIFY_LAYOUT)) == 0;
}

common::BaseLiveCaptureParameter *createParameterObject(int fd, uint32_t controlId)
{
	v4l2_queryctrl queryctrl = queryCtrl(fd, controlId);

	if ((queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) != 0)
		return nullptr; // ignore disabled controls

	if (queryctrl.type == V4L2_CTRL_TYPE_INTEGER)
	{
		return new IntegerLiveCaptureParameter(fd, controlId);
	}
	else if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
	{
		QMap<int, QString> choices = enumMenu(fd, controlId);
		if (choices.isEmpty())
			return nullptr;
		else
			return new MenuLiveCaptureParameter(fd, controlId, choices);
	}
	else
	{
		qCritical() << "Unsupported control type" << queryctrl.type << "for" << queryctrl.name;
	}
}

IntegerLiveCaptureParameter::IntegerLiveCaptureParameter(int fd, uint32_t controlId)
: common::IntegerLiveCaptureParameter((const char*)queryCtrl(fd, controlId).name)
, m_fd(fd)
, m_controlId(controlId)
{
}

bool IntegerLiveCaptureParameter::canBeRead() const
{
	return camera_V4L2::canBeRead(m_fd, m_controlId);
}

bool IntegerLiveCaptureParameter::canBeWritten() const
{
	return camera_V4L2::canBeWritten(m_fd, m_controlId);
}

void IntegerLiveCaptureParameter::setValue(int value)
{
	setCtrl(m_fd, m_controlId, value);
}

int IntegerLiveCaptureParameter::value() const
{
	return getCtrl(m_fd, m_controlId);
}

bool IntegerLiveCaptureParameter::suggestSlider() const
{
	//return (queryCtrl(m_fd, m_controlId).flags & V4L2_CTRL_FLAG_SLIDER) != 0;
	return true;
}

int IntegerLiveCaptureParameter::defaultValue() const
{
	return queryCtrl(m_fd, m_controlId).default_value;
}

int IntegerLiveCaptureParameter::minimumValue() const
{
	return queryCtrl(m_fd, m_controlId).minimum;
}

int IntegerLiveCaptureParameter::maximumValue() const
{
	return queryCtrl(m_fd, m_controlId).maximum;
}

int IntegerLiveCaptureParameter::step() const
{
	return queryCtrl(m_fd, m_controlId).step;
}

MenuLiveCaptureParameter::MenuLiveCaptureParameter(int fd, uint32_t controlId, const QMap<int, QString> &enumMenu)
: common::SingleChoiceLiveCaptureParameter((const char*)queryCtrl(fd, controlId).name, enumMenu.values())
, m_fd(fd)
, m_controlId(controlId)
, m_indexMap(enumMenu.keys())
{
}

bool MenuLiveCaptureParameter::canBeRead() const
{
	return camera_V4L2::canBeRead(m_fd, m_controlId);
}

bool MenuLiveCaptureParameter::canBeWritten() const
{
	return camera_V4L2::canBeWritten(m_fd, m_controlId);
}

void MenuLiveCaptureParameter::setCurrentChoice(int index)
{
	setCtrl(m_fd, m_controlId, m_indexMap.value(index));
}

int MenuLiveCaptureParameter::currentChoice() const
{
	return m_indexMap.indexOf(getCtrl(m_fd, m_controlId));
}

int MenuLiveCaptureParameter::defaultChoice() const
{
	return m_indexMap.indexOf(queryCtrl(m_fd, m_controlId).default_value);
}

}
