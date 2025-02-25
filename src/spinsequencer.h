#ifndef SPIN_SEQ_CLASS_H
#define SPIN_SEQ_CLASS_H
#include <closedloopcontroller.h>
#include <controller.h>
#include <jseq.h>
class SpinSequencerController
{
public:
    MotorController &motorController;
    ClosedLoopController &controller;
    JsonSeq jsonSeq;

    SpinSequencerController(MotorController &motorController, ClosedLoopController &controller):
        motorController(motorController), controller(controller), jsonSeq(motorController) {}
    
    void init() 
    {
    }

    void run()
    {
        controller.run();
        motorController.tick();
    }
};
#endif