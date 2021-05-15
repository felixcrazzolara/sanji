#pragma once

#include <QWidget>
#include <Eigen/Dense>
#include "Colors.hpp"

namespace sanji_ {

/* Type definitions */
template <typename T>
using shared_ptr = std::shared_ptr<T>;
template <typename T>
using vector     = std::vector<T>;
using VectorXd   = Eigen::VectorXd;
using MatrixXd   = Eigen::MatrixXd;
using vec_ptr    = shared_ptr<VectorXd>;
using mat_ptr    = shared_ptr<MatrixXd>;
template <class... Types>
using tuple      = std::tuple<Types...>;
using Color      = sanji::colors::Color;

// Forward declarations
struct LimitsInfo;

class RenderArea : public QWidget {

Q_OBJECT

public:

/* Constructor */
explicit RenderArea(QWidget* parent, const vector<tuple<uint,vec_ptr,mat_ptr,QPen>>* data, const LimitsInfo* limits_info);

protected:

void paintEvent(QPaintEvent* event) override;

private:

const LimitsInfo*                               limits_info_;
const vector<tuple<uint,vec_ptr,mat_ptr,QPen>>* data_;

};

};
