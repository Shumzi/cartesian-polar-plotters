#include "PlotterSystem.h"

PlotterSystem::PlotterSystem(IMotor* m1, IMotor* m2, IEncoder* e1, IEncoder* e2, IMode* md)
    : motor1(m1), motor2(m2), encoder1(e1), encoder2(e2), mode(md)
{}

void PlotterSystem::loop() {
    int delta1 = encoder1->readDelta();
    int delta2 = encoder2->readDelta();
    mode->updateEndEffector(delta1, delta2);
    motor1->run();
    motor2->run();
}

void PlotterSystem::calibrate() {
    mode->calibrate();
}