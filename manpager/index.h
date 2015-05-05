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
    class Item {
    public:
        enum {NORMAL, GZIP} mode;
        unsigned path;
        char id;
        QString name;
        QString section;
    };

private:
    QList<Index::Item> items;
    bool triggered;
    unsigned rows, first, last;
    unsigned *map;

    inline QString nameFrom(int pos) const {
        return items[map[pos]].name + "." + items[map[pos]].section;
    }

    inline QString findFrom(int pos, int len) const {
        return items[map[pos]].name.left(len);
    }

protected:
    virtual int rowCount(const QModelIndex& parent) const;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public:
    Index(QObject *parent = NULL);
    virtual ~Index();

    QString nameAt(int row) const;
    const Item& itemAt(int row) const;

    int search(const QString& name) const;
    int find(const QString& name) const;
    void selectAt(int pos, const QString& name);

signals:
    void selected(const QString& text);
};

