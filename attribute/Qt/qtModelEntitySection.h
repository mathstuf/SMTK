/*=========================================================================

Copyright (c) 1998-2003 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced, distributed,
or modified, in any form or by any means, without permission in writing from
Kitware Inc.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================*/
// .NAME qtModelEntitySection - the Attribute Section
// .SECTION Description
// .SECTION See Also
// qtSection

#ifndef __slctk_attribute_qtModelEntitySection_h
#define __slctk_attribute_qtModelEntitySection_h

#include "qtSection.h"

class qtModelEntitySectionInternals;
class QListWidgetItem;

namespace slctk
{
  namespace attribute
  {
    class SLCTKATTRIBUTE_EXPORT qtModelEntitySection : public qtSection
    {
      Q_OBJECT

    public:
      qtModelEntitySection(slctk::SectionPtr, QWidget* p);
      virtual ~qtModelEntitySection();
      QListWidgetItem* getSelectedItem();

    public slots:
      void showAdvanced(int show);
      void onShowCategory(int);
      void onListBoxSelectionChanged(QListWidgetItem * , QListWidgetItem * );

    protected:
      virtual void createWidget( );
      slctk::AttributePtr getSelectedAttribute();
      slctk::AttributePtr getAttributeFromItem(QListWidgetItem * item);
      QListWidgetItem* addAttributeListItem(slctk::AttributePtr childData);
      virtual void getAllDefinitions(
        QList<slctk::AttributeDefinitionPtr>& defs);

    private:

      qtModelEntitySectionInternals *Internals;

    }; // class
  }; // namespace attribute
}; // namespace slctk


#endif
