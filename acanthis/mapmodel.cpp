// Copyright (C) 2022, Petros Koutsolampros

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "mapmodel.h"

#include "attributeitem.h"

#include <QString>

MapModel::MapModel(QObject *parent) : QAbstractItemModel(parent), m_rootItem(new TreeItem("")) {}

QModelIndex MapModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    TreeItem *parentItem = getItem(parent);
    auto childPtr = parentItem->getChild(row);
    if (childPtr) {
        return createIndex(row, column, childPtr.get());
    } else {
        return QModelIndex();
    }
}

QModelIndex MapModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) {
        return QModelIndex();
    }
    TreeItem *childItem = getItem(index);
    auto parentPtr = childItem->getParent();
    if (!parentPtr || parentPtr == m_rootItem) {
        return QModelIndex();
    }
    return createIndex(parentPtr.get()->getRow(), 0, parentPtr.get());
}

int MapModel::rowCount(const QModelIndex &parent) const {
    TreeItem *parentItem = getItem(parent);
    return parentItem->nChildren();
}

int MapModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return 2;
}

QVariant MapModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    TreeItem *item = getItem(index);
    role = VisibilityRole;

    if (index.column() == 0) {
        role = VisibilityRole;
    } else if (index.column() == 1) {
        role = NameRole;
    }
    switch (role) {
    case NameRole:
        return item->getName();
    case VisibilityRole:
        return item->isVisible();
    default:
        break;
    }
    return QVariant();
}

QHash<int, QByteArray> MapModel::roleNames() const {
    QHash<int, QByteArray> names = QAbstractItemModel::roleNames();
    names.insert(QHash<int, QByteArray>{{VisibilityRole, "visibility"}, {NameRole, "name"}});
    return names;
}

void MapModel::resetItems() {
    beginResetModel();
    int row = 0;
    for (std::unique_ptr<MapLayer> &mapLayer : m_graphDocument->getMapLayers()) {

        auto mapItem = m_rootItem->addChildItem(QSharedPointer<MapLayer>(mapLayer.get()), row);
        mapItem->setVisible(true);
        ++row;
        if (mapLayer->hasGraph())
            auto graphItem = mapItem->addChildItem("Graph", row);
        ++row;
        auto attrItem = mapItem->addChildItem("Attributes", row);
        ++row;
        for (int col = 0; col < mapLayer->getAttributes().getNumColumns(); ++col) {
            attrItem->addChildItem(QSharedPointer<AttributeItem>(
                                       new AttributeItem(mapLayer->getAttributes().getColumn(col))),
                                   row);
            ++row;
        }

        ++row;
    }
    endResetModel();
}

void MapModel::setItemVisibility(const QModelIndex &idx, bool visibility) {
    getItem(idx)->setVisible(visibility);
    emit dataChanged(idx, idx, QVector<int>() << VisibilityRole);
}

TreeItem *MapModel::getItem(const QModelIndex &idx) const {
    if (idx.isValid()) {
        TreeItem *item = static_cast<TreeItem *>(idx.internalPointer());
        if (item) {
            return item;
        }
    }
    return m_rootItem.get();
}