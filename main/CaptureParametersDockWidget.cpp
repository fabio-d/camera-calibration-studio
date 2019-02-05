#include "main/CaptureParametersDockWidget.h"

#include "ui_CaptureParametersDockWidget.h"

namespace ccs::main
{

CaptureParametersDockWidget::CaptureParametersDockWidget(QWidget *parent)
: QDockWidget(parent)
, m_ui(new Ui_CaptureParametersDockWidget)
, m_liveCaptureCamera(nullptr)
{
	m_ui->setupUi(this);

	loadLiveCaptureParameterList();
}

CaptureParametersDockWidget::~CaptureParametersDockWidget()
{
	showNothing();

	delete m_ui;
}

void CaptureParametersDockWidget::showNothing()
{
	m_liveCaptureCamera = nullptr;
	disconnect(m_liveCaptureParametersListChanged);

	loadLiveCaptureParameterList();
}

void CaptureParametersDockWidget::showLiveCapture(common::Camera *camera)
{
	m_liveCaptureCamera = camera;
	m_liveCaptureParametersListChanged = connect(
		camera, &common::Camera::liveCaptureParameterListChanged,
		this, &CaptureParametersDockWidget::loadLiveCaptureParameterList);

	loadLiveCaptureParameterList();
}

void CaptureParametersDockWidget::loadLiveCaptureParameterList()
{
	QList<common::BaseLiveCaptureParameter*> params;
	if (m_liveCaptureCamera)
		params = m_liveCaptureCamera->liveCaptureParameterList();

	qDeleteAll(m_paramWidgets);
	m_paramWidgets.clear();

	m_ui->captureParametersStackedWidget->setCurrentIndex(!params.isEmpty());

	for (int i = 0; i < params.count(); i++)
	{
		CaptureParameterWidget *w = new CaptureParameterWidget(params[i], m_ui->liveParametersPage);
		m_paramWidgets.append(w);
		m_ui->liveParametersVerticalLayout->insertWidget(i, w);
	}
}

}
