/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/
// Copyright (c) 2008 Roberto Raggi <roberto.raggi@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef CPLUSPLUS_NAME_H
#define CPLUSPLUS_NAME_H

#include "CPlusPlusForwardDeclarations.h"


namespace CPlusPlus {

class CPLUSPLUS_EXPORT Name
{
public:
    Name();
    virtual ~Name();

    virtual const Identifier *identifier() const = 0;

    bool isNameId() const;
    bool isTemplateNameId() const;
    bool isDestructorNameId() const;
    bool isOperatorNameId() const;
    bool isConversionNameId() const;
    bool isQualifiedNameId() const;
    bool isSelectorNameId() const;

    virtual const Identifier *asNameId() const { return 0; }
    virtual const TemplateNameId *asTemplateNameId() const { return 0; }
    virtual const DestructorNameId *asDestructorNameId() const { return 0; }
    virtual const OperatorNameId *asOperatorNameId() const { return 0; }
    virtual const ConversionNameId *asConversionNameId() const { return 0; }
    virtual const QualifiedNameId *asQualifiedNameId() const { return 0; }
    virtual const SelectorNameId *asSelectorNameId() const { return 0; }

    virtual bool isEqualTo(const Name *other) const = 0;

    void accept(NameVisitor *visitor) const;
    static void accept(const Name *name, NameVisitor *visitor);

protected:
    virtual void accept0(NameVisitor *visitor) const = 0;
};

} // namespace CPlusPlus


#endif // CPLUSPLUS_NAME_H
