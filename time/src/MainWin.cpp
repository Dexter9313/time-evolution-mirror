#include "MainWin.hpp"

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
	particles = new Particles(QSettings().value("data/file").toString().toStdString().c_str());
	timer.start();

	getCamera("default").lookAt(
				QVector3D(-0.1, 0.5, 0.5),
				QVector3D(0, 0.5, 0.5),
				QVector3D(0, 0, 1)
			);
}

void MainWin::updateScene(BasicCamera& camera, QString const& /*pathId*/)
{
	time = particles->timeBegin + timer.elapsed() * (particles->timeEnd - particles->timeBegin) / 10000.0;
	particles->update(time);
}

void MainWin::renderScene(BasicCamera const& camera, QString const& /*pathId*/)
{
	particles->render(time);

	if(timer.elapsed() > 10000.0)
		timer.restart();
}

MainWin::~MainWin()
{
	delete particles;
}

