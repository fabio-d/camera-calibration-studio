#include "camera_gazebo/AddCameraWidget.h"

#include "common/Sensor.h"

#include "ui_AddCameraWidget.h"

namespace ccs::camera_gazebo
{

AddCameraWidget::AddCameraWidget(QWidget *parent)
: common::AddCameraWidget(parent)
, m_ui(new Ui_AddCameraWidget)
, m_validTopic(false)
{
	m_ui->setupUi(this);

	connect(m_ui->rescanPushButton, &QPushButton::clicked, this, &AddCameraWidget::rescanTopics);
	connect(m_ui->topicComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AddCameraWidget::topicSelected);

	rescanTopics();
}

AddCameraWidget::~AddCameraWidget()
{
	delete m_ui;
}

QJsonObject AddCameraWidget::pluginData()
{
	QJsonObject r;

	r["topic"] = m_ui->topicComboBox->currentText();
	r["width"] = m_width;
	r["height"] = m_height;

	return r;
}

QList<common::SensorStaticInfo> AddCameraWidget::sensorsStaticInfo()
{
	common::SensorStaticInfo s;

	s.name = "RGB";
	s.width = m_width;
	s.height = m_height;

	return QList<common::SensorStaticInfo>() << s;
}

QString AddCameraWidget::defaultCameraName() const
{
	return m_ui->topicComboBox->currentText();
}

bool AddCameraWidget::canAccept() const
{
	return m_validTopic;
}

void AddCameraWidget::rescanTopics()
{
	// Recreate Gazebo connection
	m_sub.reset();
	m_conn.reset();
	m_conn = GazeboConnection::connectToGazebo();

	const QString oldText = m_ui->topicComboBox->currentText();
	m_ui->topicComboBox->clear();

	if (m_conn.isNull())
	{
		m_ui->validOrNotStackedWidget->setCurrentWidget(m_ui->connectionFailedPage);

		m_validTopic = false;
		emit defaultCameraNameChanged();
		emit canAcceptChanged();
	}
	else
	{
		m_ui->topicComboBox->addItems(m_conn->imageStampedTopicList());
		int newIndex = m_ui->topicComboBox->findText(oldText);
		if (newIndex != -1)
			m_ui->topicComboBox->setCurrentIndex(newIndex);

		topicSelected();
	}
}

void AddCameraWidget::topicSelected()
{
	int topicIndex = m_ui->topicComboBox->currentIndex();
	if (topicIndex != -1)
	{
		m_ui->validOrNotStackedWidget->setCurrentWidget(m_ui->waitingForFramePage);

		// subscribe to discover image size
		GazeboSubscription *sub = new GazeboSubscription(m_conn, m_ui->topicComboBox->currentText());
		connect(sub, &GazeboSubscription::frameReceived, this, &AddCameraWidget::frameReceived);
		m_sub.reset(sub);
	}
	else
	{
		m_ui->validOrNotStackedWidget->setCurrentWidget(m_ui->noTopicPage);
	}

	m_validTopic = false;
	emit defaultCameraNameChanged();
	emit canAcceptChanged();
}

void AddCameraWidget::frameReceived(const cv::Mat &frame)
{
	// unsubscribe
	m_sub.reset();

	m_width = frame.cols;
	m_height = frame.rows;

	m_ui->widthLabel->setText(QString::number(m_width));
	m_ui->heightLabel->setText(QString::number(m_height));
	m_ui->validOrNotStackedWidget->setCurrentWidget(m_ui->gotFramePage);

	m_validTopic = true;
	emit canAcceptChanged();
}

}
