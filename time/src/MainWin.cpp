#include "MainWin.hpp"

void MainWin::keyPressEvent(QKeyEvent* e)
{
	AbstractMainWin::keyPressEvent(e);
	if(e->key() == Qt::Key_Space)
	{
		// videomode = true;
		advancing = !advancing;
		timer.start();
	}
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
}

void MainWin::updateScene(BasicCamera& camera, QString const& /*pathId*/)
{
	std::cout << 1.0 / frameTiming << std::endl;
	if(vrHandler)
	{
		camera.lookAt(QVector3D(-0.1, 0.5, 0.5), QVector3D(0, 0.5, 0.5),
		              QVector3D(0, 0, 1));
	}
	else
	{
		camera.lookAt(QVector3D(0.5, 0.5, -0.1), QVector3D(0.5, 0.5, 0),
		              QVector3D(0, 1, 0));
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
			        / 20.0;
		}
		else
		{
			if(timer.isValid())
				time = particles->timeBegin
				       + timer.elapsed()
				             * (particles->timeEnd - particles->timeBegin)
				             / 40000.0;
			else
				time = particles->timeBegin;
		}
	}

	if(time > particles->timeEnd)
	{
		advancing = false;
		videomode = false;
	}
}

void MainWin::renderScene(BasicCamera const& camera, QString const& /*pathId*/)
{
	particles->render(time);
}

MainWin::~MainWin()
{
	delete particles;
}
