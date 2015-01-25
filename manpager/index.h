// Copyright (C) 2011-2015 David Sugar, Tycho Softworks
//
// This file is part of coastal-qt.
//
// Coastal-qt is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// coastal-qt is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with coastal-qt.  If not, see <http://www.gnu.org/licenses/>.

#include <coastal.h>

class Index : public QAbstractTableModel
{
    Q_OBJECT

public:
    typedef struct {
        enum {NORMAL, GZIP} mode;
        unsigned path;
        char id;
    }   fileinfo;

private:
    QStringList names, sections;
    QList<Index::fileinfo> infos;
    bool triggered;
    unsigned rows, first, last;
    unsigned *map;

protected:
    virtual int rowCount(const QModelIndex& parent) const;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public:
    Index(QObject *parent = NULL);
    virtual ~Index();

    QString name(int row);
    fileinfo node(int row);

    int search(const QString& name);
    int find(const QString& name);
    void select(int pos, const QString& name);

signals:
    void selected(const QString& text);
};

