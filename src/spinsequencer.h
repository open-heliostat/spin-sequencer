#ifndef SPIN_SEQ_CLASS_H
#define SPIN_SEQ_CLASS_H
#include <closedloopcontroller.h>
#include <controller.h>
#include <jseq.h>
#include <CanControllerService.h>
class SpinSequencerController
{
public:
    MotorController &motorController;
    ClosedLoopController &controller;
    JsonSeq jsonSeq;
    CanIsoTPController<CanIsoTPMessage> &canController;

    SpinSequencerController(MotorController &motorController, ClosedLoopController &controller, CanIsoTPController<CanIsoTPMessage> &canController) :
        motorController(motorController), controller(controller), jsonSeq(motorController), canController(canController) {}
    
    void init() 
    {
    }

    void run()
    {
        controller.run();
        motorController.tick();
        jsonSeq.tick();
    }
};
#endif