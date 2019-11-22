#include "MainWin.hpp"

void MainWin::keyPressEvent(QKeyEvent* e)
{
	AbstractMainWin::keyPressEvent(e);
	if(e->key() == Qt::Key_Space)
	{
		videomode = true;
		advancing = !advancing;
		timer.start();
	}
	else if(e->key() == Qt::Key_Up)
	{
		height += 0.01;
	}
	else if(e->key() == Qt::Key_Down)
	{
		height -= 0.01;
	}
	else if(e->key() == Qt::Key_Left)
	{
		distance -= 0.01;
	}
	else if(e->key() == Qt::Key_Right)
	{
		distance += 0.01;
	}

	std::cout << height << " " << distance << std::endl;
}

void MainWin::actionEvent(BaseInputManager::Action a, bool pressed)
{
	AbstractMainWin::actionEvent(a, pressed);
	if(!pressed)
	{
		return;
	}
}

void MainWin::initScene()
{
	particles = new Particles(
	    QSettings().value("data/file").toString().toStdString().c_str());

	time = particles->timeBegin;

	text = new Text3D(200, 100);
	text->setText("Early Alpha\nF. Cabot, L. Hausammann");
	text->setColor(QColor(255, 255, 255));
	text->setSuperSampling(2.f);

	timeText = new Text3D(200, 50);
	timeText->setColor(QColor(255, 255, 255));
	timeText->setSuperSampling(2.f);
}

void MainWin::updateScene(BasicCamera& camera, QString const& /*pathId*/)
{
	if(vrHandler)
	{
		camera.lookAt(QVector3D(-0.1, 0.5, 0.8), QVector3D(0, 0.5, 0.8),
		              QVector3D(0, 0, 1));
	}
	else
	{
		camera.lookAt(QVector3D(0.5, 0.5, 1.0), QVector3D(0.5, 0.5, 0.),
		              QVector3D(0, 1, 0));

		/*
		 * camera.lookAt(QVector3D(0.5 * distance, 0.5 * distance, 0.5 +
		 height), QVector3D(0.5, 0.5, 0.5), QVector3D(0, 0, 1));*/
	}

	if(advancing)
	{
		// QElapsedTimer timer2;
		// timer2.start();
		particles->update(time);
		// std::cout << timer2.elapsed() / 1000.0 << std::endl;
		if(videomode)
		{
			time += frameTiming * (particles->timeEnd - particles->timeBegin)
			        / 80.0;
		}
		else
		{
			if(timer.isValid())
				time = particles->timeBegin
				       + timer.elapsed()
				             * (particles->timeEnd - particles->timeBegin)
				             / 240000.0;
			else
				time = particles->timeBegin;
		}
	}

	if(time - particles->timeBegin
	   > (particles->timeEnd - particles->timeBegin) / 4.0)
	{
		advancing = false;
		videomode = false;
	}
	else
	{
		std::cout << 400.f * (time - particles->timeBegin)
		                 / (particles->timeEnd - particles->timeBegin)
		          << "%" << std::endl;
	}

	QMatrix4x4 model;
	model.translate(-0.5, 0.75, 0.0);

	text->getModel() = camera.screenToWorldTransform() * model;

	QMatrix4x4 timeModel;
	timeModel.translate(1.0, -1.0, 0.0);
	unsigned int precision(3);
	for(double i(10.0); i < 10000.0; i *= 10.0)
	{
		if(time * 1000.0 > i)
		{
			--precision;
		}
	}

	QString timeStr(QString::number(time*1000.0, 'f', precision));
	timeText->setText(timeStr + " Myr");
	timeText->getModel() = camera.screenToWorldTransform() * timeModel;
}

void MainWin::renderScene(BasicCamera const& /*camera*/, QString const& /*pathId*/)
{
	particles->render(time);
	text->render();
	timeText->render();
}

MainWin::~MainWin()
{
	delete particles;
	delete text;
	delete timeText;
}
