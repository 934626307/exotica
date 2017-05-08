/*
 *  Created on: 19 Jun 2014
 *      Author: Vladimir Ivan
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

#ifndef SAMPLINGPROBLEM_H_
#define SAMPLINGPROBLEM_H_

#include <exotica/PlanningProblem.h>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include "exotica/Definitions/TaskTerminationCriterion.h"
#include <exotica/SamplingProblemInitializer.h>

namespace exotica
{

  enum SamplingProblem_Type
  {
    OMPL_PROBLEM_GOAL = 0,
    OMPL_PROBLEM_COSTS,
    OMPL_PROBLEM_GOAL_BIAS,
    OMPL_PROBLEM_SAMPLING_BIAS
  };

  class SamplingProblem: public PlanningProblem, public Instantiable<SamplingProblemInitializer>
  {
    public:
      SamplingProblem();
      virtual
      ~SamplingProblem();

      virtual void Instantiate(SamplingProblemInitializer& init);

      int getSpaceDim();

      boost::mutex& getLock()
      {
        return update_lock_;
      }

      std::vector<TaskTerminationCriterion_ptr>& getGoals();
      std::vector<double>& getBounds();
      bool isCompoundStateSpace();
      virtual void reinitialise(rapidjson::Document& document,
          boost::shared_ptr<PlanningProblem> problem);
      std::string local_planner_config_;
      EParam<std_msgs::Bool> full_body_plan_;

      SamplingProblemInitializer Parameters;

      virtual void clear(bool keepOriginals = true);
    private:
      boost::mutex update_lock_;
      std::vector<TaskTerminationCriterion_ptr> goals_;
      std::vector<double> bounds_;
      int space_dim_;
      SamplingProblem_Type problemType;
      bool compound_;
      std::vector<TaskTerminationCriterion_ptr> originalGoals_;

  };

  typedef boost::shared_ptr<exotica::SamplingProblem> SamplingProblem_ptr;

}

#endif
