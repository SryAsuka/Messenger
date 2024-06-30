#pragma once

#include <QAbstractItemModel>

class UserListModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit UserListModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

// class UserListModelSingleton : public QObject
// {
//     Q_OBJECT
// public:
//     Q_PROPERTY(UserListModel *userlistModel READ userlistModel CONSTANT)
//     static UserListModelSingleton *instance()
//     {
//         static UserListModelSingleton instance;
//         return instance;
//     }

// private:
//     UserListModelSingleton() {} // Private constructor to prevent direct instantiation
//     UserListModel *m_myModel = new UserListModel; // 这里使用自定义的 MyListModel
//     UserListModel *userlistModel() const { return m_myModel; }
// };
