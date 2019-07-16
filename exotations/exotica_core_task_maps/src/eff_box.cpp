//
// Copyright (c) 2019, Christopher E. Mower
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of  nor the names of its contributors may be used to
//    endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include <exotica_core_task_maps/eff_box.h>

REGISTER_TASKMAP_TYPE("EffBox", exotica::EffBox);

namespace exotica
{
void EffBox::Update(Eigen::VectorXdRefConst x, Eigen::VectorXdRef phi)
{
    if (phi.rows() != TaskSpaceDim()) ThrowNamed("Wrong size of phi!");

    Eigen::VectorXd e;
    e.resize(three_times_n_effs_, 1);

    for (int i = 0; i < n_effs_; ++i)
    {
        const int eff_id = 3 * i;
        e.segment(eff_id, 3) = Eigen::Map<Eigen::Vector3d>(kinematics[i].Phi(i).p.data);
    }

    phi.topRows(three_times_n_effs_) = e - eff_upper_;
    phi.bottomRows(three_times_n_effs_) = eff_lower_ - e;
}

void EffBox::Update(Eigen::VectorXdRefConst x, Eigen::VectorXdRef phi, Eigen::MatrixXdRef jacobian)
{
    if (phi.rows() != TaskSpaceDim()) ThrowNamed("Wrong size of phi!");
    if (jacobian.rows() != TaskSpaceDim() || jacobian.cols() != kinematics[0].jacobian(0).data.cols()) ThrowNamed("Wrong size of jacobian! " << kinematics[0].jacobian(0).data.cols());

    Eigen::VectorXd e;
    e.resize(three_times_n_effs_, 1);

    Eigen::MatrixXd ed;
    ed.resize(three_times_n_effs_, jacobian.cols());

    for (int i = 0; i < n_effs_; ++i)
    {
        const int eff_id = 3 * i;
        e.segment(eff_id, 3) = Eigen::Map<Eigen::Vector3d>(kinematics[i].Phi(i).p.data);
        ed.middleRows(eff_id, 3) = kinematics[0].jacobian(i).data.topRows<3>();
    }

    phi.topRows(three_times_n_effs_) = e - eff_upper_;
    phi.bottomRows(three_times_n_effs_) = eff_lower_ - e;

    jacobian.topRows(three_times_n_effs_) = ed;
    jacobian.bottomRows(three_times_n_effs_) = -ed;
}

const Eigen::VectorXd EffBox::GetLowerLimit()
{
    return eff_lower_;
}

const Eigen::VectorXd EffBox::GetUpperLimit()
{
    return eff_upper_;
}

void EffBox::Instantiate(const EffBoxInitializer& init)
{
    parameters_ = init;
    n_effs_ = frames_.size();
    three_times_n_effs_ = 3 * n_effs_;

    // Populate eff_upper_ and eff_lower_
    eff_upper_.resize(3 * n_effs_, 1);
    eff_lower_.resize(3 * n_effs_, 1);

    for (int i = 0; i < n_effs_; ++i)
    {
        const int eff_id = 3 * i;

        // Initialize frame and check user input
        FrameWithBoxLimitsInitializer frame(parameters_.EndEffector[i]);
        if (frame.XLim[0] > frame.XLim[1]) ThrowPretty("Specify XLim using lower then upper for end-effector " << i << ".");
        if (frame.YLim[0] > frame.YLim[1]) ThrowPretty("Specify YLim using lower then upper for end-effector " << i << ".");
        if (frame.ZLim[0] > frame.ZLim[1]) ThrowPretty("Specify ZLim using lower then upper for end-effector " << i << ".");

        // Set upper and lower limits
        eff_upper_[eff_id] = frame.XLim[1];
        eff_upper_[eff_id + 1] = frame.YLim[1];
        eff_upper_[eff_id + 2] = frame.ZLim[1];

        eff_lower_[eff_id] = frame.XLim[0];
        eff_lower_[eff_id + 1] = frame.YLim[0];
        eff_lower_[eff_id + 2] = frame.ZLim[0];
    }
}

int EffBox::TaskSpaceDim()
{
    return 6 * n_effs_;
}
}  // namespace exotica
