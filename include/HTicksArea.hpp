#pragma once

#include <QWidget>
#include <QLabel>
#include <vector>

namespace sanji_ {

/* Type definitions */
template <typename T>
using vector     = std::vector<T>;

// Forward declarations
struct LimitsInfo;

class HTicksArea : public QWidget {

Q_OBJECT

public:

/* Constructor */
explicit HTicksArea(const LimitsInfo* limits_info);

protected:

void paintEvent(QPaintEvent* event) override;

private:

const LimitsInfo* limits_info_;
vector<QLabel*>   tick_labels_;

};

};
