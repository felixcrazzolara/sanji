#include <QFontDatabase>
#include <QDebug>
#include <vector>
#include "Sanji.hpp"

namespace sanji_ {

/* Type definitions */
template <typename T>
using vector = std::vector<T>;

/* Extern declarations */
extern int                gbl_current_fig_index;
extern vector<sh_fig_ptr> gbl_fig_holder;
extern vector<bool>       gbl_free_fig_indices;

};

namespace sanji {

using namespace sanji_;

void init() {
    /* Initialize global variables */
    gbl_current_fig_index = -1;
    gbl_free_fig_indices  = vector<bool>(10,true);
    gbl_fig_holder.resize(10);
}

sh_fig_ptr figure() {
    return figure("");
}

sh_fig_ptr figure(const QString fig_name) {
    const sh_fig_ptr fig_ptr = std::make_shared<Figure>(fig_name);
    gbl_fig_holder[gbl_current_fig_index] = fig_ptr;
    return fig_ptr;
}

void plot(const VectorXd& x, const MatrixXd& y, const int priority, const char line_style, const Color color) {
    if (x.rows() == 0) return;

    // Make sure that a figure is available
    if (gbl_current_fig_index == -1) figure();

    // Plot the data
    gbl_fig_holder[gbl_current_fig_index]->plot(x,y,priority,line_style,color);
}

void quiver(const VectorXd& x, const VectorXd& y, const VectorXd& u, const VectorXd& v, const int priority, const char line_style, const Color color) {
    if (x.rows() == 0) return;

    // Make sure that a figure is available
    if (gbl_current_fig_index == -1) figure();

    // Plot the data
    gbl_fig_holder[gbl_current_fig_index]->quiver(x,y,u,v,priority,line_style,color);
}

void printAvailableFontFamilies() {
    const QFontDatabase fdb;
    qInfo() << fdb.families();
}

void setAxisRatio(const std::string& axis_ratio) {
    gbl_fig_holder[gbl_current_fig_index]->setAxisRatio(axis_ratio);
}

};
