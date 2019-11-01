#ifndef MAINWIN_H
#define MAINWIN_H

#include <QElapsedTimer>

#include "AbstractMainWin.hpp"
#include "Particles.hpp"

class MainWin : public AbstractMainWin
{
	Q_OBJECT
  public:
	MainWin() = default;
	virtual ~MainWin();

  protected:
	virtual void keyPressEvent(QKeyEvent* e) override;
	virtual void actionEvent(BaseInputManager::Action a, bool pressed) override;

	// declare drawn resources
	virtual void initScene() override;

	// update physics/controls/meshes, etc...
	// prepare for rendering
	virtual void updateScene(BasicCamera& camera,
	                         QString const& pathId) override;

	// render user scene on camera
	// (no controllers or hands)
	virtual void renderScene(BasicCamera const& camera,
	                         QString const& pathId) override;

  private:
	Particles* particles;

	QElapsedTimer timer;
	double time = 0.0;

	bool advancing = false;
};

#endif // MAINWIN_H
