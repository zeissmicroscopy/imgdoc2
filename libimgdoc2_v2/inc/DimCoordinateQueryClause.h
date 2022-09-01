#pragma once

#include "IDimCoordinateQueryClause.h"
#include <vector>
#include <map>

namespace imgdoc2
{
  /// A simple implementation of the IDimCoordinateQueryClause interface.
  class CDimCoordinateQueryClause : public IDimCoordinateQueryClause
  {
  private:
    std::map<imgdoc2::Dimension, std::vector<RangeClause>> rangeClauses;
    std::set<imgdoc2::Dimension> dims;
  public:
    void AddRangeClause(imgdoc2::Dimension d, const RangeClause& clause)
    {
      this->rangeClauses[d].push_back(clause);
      this->dims.insert(d);
    }

    const std::set<imgdoc2::Dimension>& GetTileDimsForClause() const override
    {
      return this->dims;
    }

    const std::vector<RangeClause>* GetRangeClause(imgdoc2::Dimension d) const override
    {
      const auto& c = this->rangeClauses.find(d);
      if (c != this->rangeClauses.cend())
      {
        return &c->second;
      }

      return nullptr;
    }
  };
}
