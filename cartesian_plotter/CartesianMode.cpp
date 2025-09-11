class CartesianMode : public IMode {
private:
    IMotor* xMotor;
    IMotor* yMotor;
    long canvasWidth, canvasHeight;
    long x, y; // current end effector position
public:
    CartesianMode(IMotor* xm, IMotor* ym, long w, long h)
        : xMotor(xm), yMotor(ym), canvasWidth(w), canvasHeight(h), x(0), y(0) {}

    bool updateEndEffector(int dx, int dy) override {
        long nx = x + dx;
        long ny = y + dy;
        if (nx < 0 || nx > canvasWidth || ny < 0 || ny > canvasHeight)
            return false; // out of bounds
        x = nx;
        y = ny;
        xMotor->moveTo(x);
        yMotor->moveTo(y);
        return true;
    }

    std::pair<long,long> getMotorTargets() override {
        return {x, y};
    }
};