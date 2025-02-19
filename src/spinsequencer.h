#ifndef SPIN_SEQ_CLASS_H
#define SPIN_SEQ_CLASS_H
#include <closedloopcontroller.h>
class SpinSequencerController
{
public:
    SpinSequencerController(ClosedLoopController motorController):
        motorController(motorController) {}
    ClosedLoopController motorController;
    
    void init() 
    {
    }

    void run()
    {
        motorController.run();
    }
};
#endif