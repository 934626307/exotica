/*
 *  Created on: 2 Mar 2016
 *      Author: Yiming Yang
 *
 * Copyright (c) 2016, University Of Edinburgh
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of  nor the names of its contributors may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "ompl_imp_solver/OMPLBaseStateSpace.h"

namespace exotica
{
  OMPLBaseStateSpace::OMPLBaseStateSpace(unsigned int dim,
      const Server_ptr &server, SamplingProblem_ptr &prob)
      : ob::CompoundStateSpace(), server_(Server::Instance()), prob_(prob)
  {
  }

  OMPLStateValidityChecker::OMPLStateValidityChecker(
      const ob::SpaceInformationPtr &si, const SamplingProblem_ptr &prob)
      : ob::StateValidityChecker(si), prob_(prob)
  {
    Server_ptr server = Server::Instance();
    if (server->hasParam(server->getName() + "/SafetyMargin"))
      server->getParam(server->getName() + "/SafetyMargin", margin_);
    else
    {
      margin_.reset(new std_msgs::Float64());
      margin_->data = 0.0;
      server->setParam(server->getName() + "/SafetyMargin", margin_);
    }
    if (server->hasParam(server->getName() + "/SelfCollisionCheck"))
      server->getParam(server->getName() + "/SelfCollisionCheck", self_collision_);
    else
    {
      self_collision_.reset(new std_msgs::Bool());
      self_collision_->data = false;
      server->setParam(server->getName() + "/SelfCollisionCheck", self_collision_);
    }

    HIGHLIGHT_NAMED("OMPLStateValidityChecker",
        "Safety Margin: " << margin_->data);
  }

  bool OMPLStateValidityChecker::isValid(const ompl::base::State *state) const
  {
    double tmp;
    return isValid(state, tmp);
  }

  bool OMPLStateValidityChecker::isValid(const ompl::base::State *state,
      double &dist) const
  {
    Eigen::VectorXd q(prob_->getSpaceDim());
    boost::static_pointer_cast<OMPLBaseStateSpace>(si_->getStateSpace())->OMPLToExoticaState(
        state, q);
    boost::mutex::scoped_lock lock(prob_->getLock());
    prob_->update(q, 0);
    if (!prob_->getScene()->getCollisionScene()->isStateValid(self_collision_->data, margin_->data))
    {
      dist = -1;
      return false;
    }
    return true;
  }
}

