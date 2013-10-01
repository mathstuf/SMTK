/*=========================================================================

Copyright (c) 1998-2012 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced, distributed,
or modified, in any form or by any means, without permission in writing from
Kitware Inc.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO
PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================*/
// .NAME AttributeSection.h -
// .SECTION Description
// .SECTION See Also

#ifndef __AttributeSection_h
#define __AttributeSection_h

#include "smtk/attribute/Section.h"
#include "smtk/SMTKCoreExports.h"
#include "smtk/PublicPointerDefs.h"
#include <string>
#include <vector>

namespace smtk
{
  namespace attribute
  {
    class SMTKCORE_EXPORT AttributeSection : public Section
    {
    public:
      static smtk::AttributeSectionPtr New(const std::string &myTitle)
      { return smtk::AttributeSectionPtr(new AttributeSection(myTitle)); }

      AttributeSection(const std::string &myTitle);
      virtual ~AttributeSection();
      virtual Section::Type type() const;
      void addDefinition(smtk::AttributeDefinitionPtr def)
      {this->m_definitions.push_back(def);}
      std::size_t numberOfDefinitions() const
      { return this->m_definitions.size();}
      smtk::AttributeDefinitionPtr definition(int ith) const
      {return this->m_definitions[ith];}
      unsigned long modelEntityMask() const
      {return this->m_modelEntityMask;}
      void setModelEntityMask(unsigned long mask)
      {this->m_modelEntityMask = mask;}
      bool okToCreateModelEntities() const
      { return this->m_okToCreateModelEntities;}
      void setOkToCreateModelEntities(bool val)
      { this->m_okToCreateModelEntities = val;}

    protected:
      std::vector<smtk::AttributeDefinitionPtr> m_definitions;
      unsigned long m_modelEntityMask;
      bool m_okToCreateModelEntities;

    private:
    };
  }
}


#endif /* __AttributeSection_h */
