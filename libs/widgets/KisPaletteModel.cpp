/*
 *  Copyright (c) 2013 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "KisPaletteModel.h"

#include <QBrush>
#include <QDomDocument>
#include <QDomElement>
#include <QMimeData>

#include <KoColor.h>

#include <KoColorSpace.h>
#include <KoColorModelStandardIds.h>
#include <resources/KoColorSet.h>
#include <KoColorDisplayRendererInterface.h>

KisPaletteModel::KisPaletteModel(QObject* parent)
    : QAbstractTableModel(parent)
    , m_colorSet(0)
    , m_displayRenderer(KoDumbColorDisplayRenderer::instance())
{
}

KisPaletteModel::~KisPaletteModel()
{
}

QVariant KisPaletteModel::data(const QModelIndex& index, int role) const
{
    bool groupNameRow = m_groupNameRows.contains(index.row());
    if (role == IsGroupNameRole) {
        return groupNameRow;
    }
    if (groupNameRow) {
        return dataForGroupNameRow(index, role);
    } else {
        return dataForSwatch(index, role);
    }
}

int KisPaletteModel::rowCount(const QModelIndex& /*parent*/) const
{
    if (!m_colorSet)
        return 0;
    return m_colorSet->rowCount() // count of color rows
            + m_groupNameRows.size()  // rows for names
            - 1; // global doesn't have a name
}

int KisPaletteModel::columnCount(const QModelIndex& /*parent*/) const
{
    if (m_colorSet && m_colorSet->columnCount() > 0) {
        return m_colorSet->columnCount();
    }
    if (!m_colorSet) {
        return 0;
    }
    return 16;
}

Qt::ItemFlags KisPaletteModel::flags(const QModelIndex& index) const
{
    if (index.isValid()) {
        return  Qt::ItemIsSelectable |
                Qt::ItemIsEnabled |
                Qt::ItemIsUserCheckable |
                Qt::ItemIsDragEnabled |
                Qt::ItemIsDropEnabled;
    }
    return Qt::ItemIsDropEnabled;
}

QModelIndex KisPaletteModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    Q_ASSERT(m_colorSet);
    KisSwatchGroup *group = Q_NULLPTR;
    int groupNameRow = groupNameRowForRow(row);
    group = m_colorSet->getGroup(m_groupNameRows[groupNameRow]);
    Q_ASSERT(group);
    return createIndex(row, column, group);
}

void KisPaletteModel::setColorSet(KoColorSet* colorSet)
{
    beginResetModel();
    m_groupNameRows.clear();
    m_colorSet = colorSet;
    int row = -1;
    for (const QString &groupName : m_colorSet->getGroupNames()) {
        m_groupNameRows[row] = groupName;
        row += m_colorSet->getGroup(groupName)->rowCount();
        row += 1; // row for group name
    }
    endResetModel();
}

KoColorSet* KisPaletteModel::colorSet() const
{
    return m_colorSet;
}

int KisPaletteModel::rowNumberInGroup(int rowInModel) const
{
    if (m_groupNameRows.contains(rowInModel)) {
        return -1;
    }
    for (auto it = m_groupNameRows.keys().rbegin(); it != m_groupNameRows.keys().rend(); it++) {
        if (*it < rowInModel) {
            return rowInModel - *it - 1;
        }
    }
    return rowInModel;
}

bool KisPaletteModel::addEntry(const KisSwatch &entry, const QString &groupName)
{
    KisSwatchGroup *group = m_colorSet->getGroup(groupName);
    beginResetModel();
    if (group->checkEntry(m_colorSet->columnCount(), group->rowCount())) {
        m_colorSet->add(entry, groupName);
    } else {
        m_colorSet->add(entry, groupName);
    }
    endResetModel();
    m_colorSet->save();
    return true;
}

bool KisPaletteModel::removeEntry(const QModelIndex &index, bool keepColors)
{
    if (!qvariant_cast<bool>(data(index, IsGroupNameRole))) {
        static_cast<KisSwatchGroup*>(index.internalPointer())->removeEntry(index.column(),
                                                                           rowNumberInGroup(index.row()));
        emit dataChanged(index, index);
    } else {
        beginResetModel();
        int groupNameRow = groupNameRowForRow(index.row());
        QString groupName = m_groupNameRows[groupNameRow];
        m_colorSet->removeGroup(groupName, keepColors);
        m_groupNameRows.remove(groupNameRow);
        endResetModel();
    }
    return true;
}

bool KisPaletteModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                   int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!data->hasFormat("krita/x-colorsetentry") && !data->hasFormat("krita/x-colorsetgroup")) {
        return false;
    }
    if (action == Qt::IgnoreAction) {
        return false;
    }

    if (data->hasFormat("krita/x-colorsetgroup")) {
        // dragging group not supported for now
        /*
        QByteArray encodedData = data->data("krita/x-colorsetgroup");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);

        while (!stream.atEnd()) {
            QString groupName;
            stream >> groupName;
            QModelIndex index = this->index(endRow, 0);
            if (index.isValid()) {
                QStringList entryList = qvariant_cast<QStringList>(index.data(RetrieveEntryRole));
                QString groupDroppedOn = QString();
                if (!entryList.isEmpty()) {
                    groupDroppedOn = entryList.at(0);
                }
                int groupIndex = colorSet()->getGroupNames().indexOf(groupName);
                beginMoveRows(  QModelIndex(), groupIndex, groupIndex, QModelIndex(), endRow);
                m_colorSet->moveGroup(groupName, groupDroppedOn);
                m_colorSet->save();
                endMoveRows();

                ++endRow;
            }
        */
        return true;
    }

    QModelIndex finalIdx = parent;
    if (!finalIdx.isValid()) { return false; }
    qDebug() << "KisPaletteModel::dropMimedata" << finalIdx.row();
    qDebug() << "KisPaletteModel::dropMimedata" << finalIdx.column();
    qDebug() << "KisPaletteModel::dropMimedata" << qvariant_cast<QString>(finalIdx.data(KisPaletteModel::GroupNameRole));

    if (qvariant_cast<bool>(finalIdx.data(KisPaletteModel::IsGroupNameRole))) {
        return true;
    }
    QByteArray encodedData = data->data("krita/x-colorsetentry");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        KisSwatch entry;

        QString name, id;
        bool spotColor;
        QString oldGroupName;
        int oriRow;
        int oriColumn;
        QString colorXml;

        stream >> name >> id >> spotColor
                >> oriRow >> oriColumn
                >> oldGroupName
                >> colorXml;

        entry.setName(name);
        entry.setId(id);
        entry.setSpotColor(spotColor);

        QDomDocument doc;
        doc.setContent(colorXml);
        QDomElement e = doc.documentElement();
        QDomElement c = e.firstChildElement();
        if (!c.isNull()) {
            QString colorDepthId = c.attribute("bitdepth", Integer8BitsColorDepthID.id());
            entry.setColor(KoColor::fromXML(c, colorDepthId));
        }

        if (action == Qt::MoveAction){
            KisSwatchGroup *g = m_colorSet->getGroup(oldGroupName);
            qDebug() << oldGroupName;
            qDebug() << g;
            if (g) {
                if (qvariant_cast<bool>(finalIdx.data(KisPaletteModel::CheckSlotRole))) {
                    g->setEntry(getEntry(finalIdx), oriColumn, oriRow);
                } else {
                    qDebug() << oriColumn << oriRow;
                    qDebug() << g->removeEntry(oriColumn, oriRow);
                }
            }
            setEntry(entry, finalIdx);
            emit dataChanged(finalIdx, finalIdx);
            if (m_colorSet->isGlobal()) {
                m_colorSet->save();
            }
        }
    }

    return true;
}

QMimeData *KisPaletteModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QString mimeTypeName = "krita/x-colorsetentry";
    QModelIndex index = indexes.last();
    if (index.isValid()) {
        if (qvariant_cast<bool>(index.data(IsGroupNameRole))==false) {
            KisSwatch entry = getEntry(index);

            QDomDocument doc;
            QDomElement root = doc.createElement("Color");
            root.setAttribute("bitdepth", entry.color().colorSpace()->colorDepthId().id());
            doc.appendChild(root);
            entry.color().toXML(doc, root);

            stream << entry.name() << entry.id() << entry.spotColor()
                   << rowNumberInGroup(index.row()) << index.column()
                   << qvariant_cast<QString>(index.data(KisPaletteModel::GroupNameRole))
                   << doc.toString();
        } else {
            mimeTypeName = "krita/x-colorsetgroup";
            QStringList entryList = qvariant_cast<QStringList>(index.data(RetrieveEntryRole));
            QString groupName = QString();
            if (!entryList.isEmpty()) {
                groupName = entryList.at(0);
            }
            stream << groupName;
        }
    }

    mimeData->setData(mimeTypeName, encodedData);
    return mimeData;
}

QStringList KisPaletteModel::mimeTypes() const
{
    return QStringList() << "krita/x-colorsetentry" << "krita/x-colorsetgroup";
}

Qt::DropActions KisPaletteModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

void KisPaletteModel::setEntry(const KisSwatch &entry,
                               const QModelIndex &index)
{
    KisSwatchGroup *group = static_cast<KisSwatchGroup*>(index.internalPointer());
    Q_ASSERT(group);
    group->setEntry(entry, index.column(), rowNumberInGroup(index.row()));
    emit dataChanged(index, index);
    if (m_colorSet->isGlobal()) {
        m_colorSet->save();
    }
}

bool KisPaletteModel::renameGroup(const QString &groupName, const QString &newName)
{
    beginResetModel();
    bool success = m_colorSet->changeGroupName(groupName, newName);
    for (auto it = m_groupNameRows.begin(); it != m_groupNameRows.end(); it++) {
        if (it.value() == groupName) {
            m_groupNameRows[it.key()] = newName;
            break;
        }
    }
    endResetModel();
    return success;
}

QVariant KisPaletteModel::dataForGroupNameRow(const QModelIndex &idx, int role) const
{
    KisSwatchGroup *group = static_cast<KisSwatchGroup*>(idx.internalPointer());
    Q_ASSERT(group);
    QString groupName = group->name();
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole: {
        return groupName;
    }
    case GroupNameRole: {
        return groupName;
    }
    case CheckSlotRole: {
        return true;
    }
    case RowInGroupRole: {
        return -1;
    }
    default: {
        return QVariant();
    }
    }
}

QVariant KisPaletteModel::dataForSwatch(const QModelIndex &idx, int role) const
{
    KisSwatchGroup *group = static_cast<KisSwatchGroup*>(idx.internalPointer());
    Q_ASSERT(group);
    int rowInGroup = rowNumberInGroup(idx.row());
    bool entryPresent = group->checkEntry(idx.column(), rowInGroup);
    KisSwatch entry;
    if (entryPresent) {
        entry = group->getEntry(idx.column(), rowInGroup);
    }
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole: {
        return entryPresent ? entry.name() : i18n("Empty slot");
    }
    case Qt::BackgroundRole: {
        QColor color(0, 0, 0, 0);
        if (entryPresent) {
            color = m_displayRenderer->toQColor(entry.color());
        }
        return QBrush(color);
    }
    case GroupNameRole: {
        return group->name();
    }
    case CheckSlotRole: {
        return entryPresent;
    }
    case RowInGroupRole: {
        return rowInGroup;
    }
    default: {
        return QVariant();
    }
    }
}

void KisPaletteModel::setDisplayRenderer(const KoColorDisplayRendererInterface *displayRenderer)
{
    if (displayRenderer) {
        if (m_displayRenderer) {
            disconnect(m_displayRenderer, 0, this, 0);
        }
        m_displayRenderer = displayRenderer;
        connect(m_displayRenderer, SIGNAL(displayConfigurationChanged()),
                SLOT(slotDisplayConfigurationChanged()));
    } else {
        m_displayRenderer = KoDumbColorDisplayRenderer::instance();
    }
}

void KisPaletteModel::slotDisplayConfigurationChanged()
{
    beginResetModel();
    endResetModel();
}

QModelIndex KisPaletteModel::indexForClosest(const KoColor &compare)
{
    KisSwatchGroup::SwatchInfo info = colorSet()->getClosestColorInfo(compare);
    return createIndex(indexRowForInfo(info), info.column, colorSet()->getGroup(info.group));
}

int KisPaletteModel::indexRowForInfo(const KisSwatchGroup::SwatchInfo &info)
{
    for (auto it = m_groupNameRows.begin(); it != m_groupNameRows.end(); it++) {
        if (it.value() == info.group) {
            return it.key() + info.row + 1;
        }
    }
    return info.row;
}

KisSwatch KisPaletteModel::getEntry(const QModelIndex &index) const
{
    KisSwatchGroup *group = static_cast<KisSwatchGroup*>(index.internalPointer());
    if (!group || !group->checkEntry(index.column(), rowNumberInGroup(index.row()))) {
        return KisSwatch();
    }
    return group->getEntry(index.column(), rowNumberInGroup(index.row()));
}

int KisPaletteModel::groupNameRowForRow(int rowInModel) const
{
    return rowInModel - rowNumberInGroup(rowInModel) - 1;
}
