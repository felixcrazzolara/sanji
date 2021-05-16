#pragma once

#include <QWidget>
#include <QPen>
#include <memory>
#include <vector>
#include <tuple>
#include <string>
#include <unordered_map>
#include <Eigen/Dense>
#include "RenderArea.hpp"
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
using Style      = std::unordered_map<std::string,int>;

struct LimitsInfo {
    double xmin;
    double xmax;
    double ymin;
    double ymax;

    double xmin_value;
    double xmax_value;
    double ymin_value;
    double ymax_value;
    
    bool   value_init;
    
    bool   xmin_set;
    bool   xmax_set;
    bool   ymin_set;
    bool   ymax_set;
};

class Figure : public QWidget {

Q_OBJECT

public:

/* Constructor and destructor */
Figure(const QString& fig_name="");
virtual ~Figure();

/* Miscellaneous */
void plot(const VectorXd& x, const MatrixXd& y, const Style& style, const int priority);
void quiver(const VectorXd& x, const VectorXd& y, const VectorXd& u, const VectorXd& v, const Style& style, const int priority);
void setAxisRatio(const std::string& axis_ratio);

protected:

void resizeEvent(QResizeEvent* event) override;

private:

friend RenderArea;

void checkRenderArea();

QString                                                            fig_name_;
int                                                                current_render_area_idx_;
vector<RenderArea*>                                                render_areas_;
vector<LimitsInfo>                                                 limits_info_;
vector<vector<tuple<uint,vec_ptr,mat_ptr,QPen>>>                   line_data_;
vector<vector<tuple<uint,vec_ptr,vec_ptr,vec_ptr,vec_ptr,QBrush>>> arrow_data_;

};

/* Type definitions */
using sh_fig_ptr = shared_ptr<Figure>;

};
