#pragma once

#include "IDimCoordinateQueryClause.h"
#include <vector>
#include <map>

namespace imgdoc2
{
  class CDimCoordinateQueryClause : public IDimCoordinateQueryClause
  {
  private:
    std::map<imgdoc2::Dimension, std::vector<RangeClause>> rangeClauses;
    std::map<imgdoc2::Dimension, std::vector<ListClause>> listClauses;
    std::set<imgdoc2::Dimension> dims;
  public:
    void AddRangeClause(imgdoc2::Dimension d, const RangeClause& clause)
    {
      this->rangeClauses[d].push_back(clause);
      this->dims.insert(d);
    }

    void AddListClause(imgdoc2::Dimension d, const ListClause& clause)
    {
      this->listClauses[d].push_back(clause);
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

    /*const std::vector<ListClause>* GetListClause(imgdoc2::Dimension d) const override
    {
      const auto& c = this->listClauses.find(d);
      if (c != this->listClauses.cend())
      {
        return &c->second;
      }

      return nullptr;
    }*/
  };
}
