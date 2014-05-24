import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.moviesme.myPrivateImports 1.0
import "../components"

Page {
    id: page

    SilicaGridView {
        id: view;
        cellWidth: (width * 0.25);
        cellHeight: (coverHeight * cellWidth / coverWidth);
        model: engine.sortedFilteredMoviesModel;
        header: PageHeader {
            title: qsTr ("Movies'me");
            width: view.width;
        }
        delegate: ListItem {
            id: itemMovie;
            width: view.cellWidth;
            contentHeight: view.cellHeight;
            menu: Component {
                ContextMenu {
                    MenuItem {
                        text: qsTr ("Remove this movie");
                        onClicked: { remove (); }
                    }
                }
            }
            onClicked: {
                engine.currentMovieId = model.tmdbId;
                pageStack.pushAttached (movieDetailsPage);
                pageStack.navigateForward ();
            }

            function remove () {
                var tmp = model.tmdbId;
                remorse.execute (itemMovie,
                                 qsTr ("Deleting"),
                                 function () {
                                     engine.requestRemoveMovie (tmp);
                                 });
            }

            RemorseItem { id: remorse; }
            CachedImage {
                id: imgCover;
                opacity: (itemMovie.highlighted ? 0.85 : 1.0);
                source: model.cover;
                anchors.fill: parent;
            }
            Rectangle {
                color: "transparent";
                border {
                    width: 1.
                    color: "black";
                }
                anchors.fill: parent;
            }
        }
        footer: Item {
            width: view.width;
            height: dock.height;
        }
        anchors.fill: parent;

        PullDownMenu {
            MenuItem {
                text: qsTr ("Add a new movie");
                onClicked: {
                    pageStack.pushAttached (moviesSearchAndAddPage);
                    pageStack.navigateForward ();
                }
            }
        }
        ViewPlaceholder {
            text: qsTr ("No movie. Use menu to add some...");
            enabled: !view.count;
        }
        VerticalScrollDecorator {}
    }
    DockedPanel {
        id: dock;
        open: true;
        dock: Dock.Bottom;
        height: Theme.itemSizeMedium;
        anchors {
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }

        Rectangle {
            color: "black";
            opacity: 0.65;
            anchors.fill: parent;
        }
        Rectangle {
            color: "white";
            opacity: 0.05;
            anchors.fill: (engine.sortedFilteredMoviesModel.toggleFilterSeen ? tabSeen : tabWatchList);
        }
        MouseArea {
            id: tabSeen;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.horizontalCenter;
                bottom: parent.bottom;
            }
            onClicked: { engine.sortedFilteredMoviesModel.toggleFilterSeen = true; }

            property bool active : engine.sortedFilteredMoviesModel.toggleFilterSeen;

            Row {
                spacing: Theme.paddingSmall;
                anchors.centerIn: parent;

                Image {
                    width: height;
                    height: (Theme.itemSizeMedium * 0.5);
                    source: "image://theme/icon-l-star?%1".arg (lblSeen.color.toString ());
                    anchors.verticalCenter: parent.verticalCenter;
                }
                Label {
                    id: lblSeen;
                    text: qsTr ("Seen");
                    color: (tabSeen.active ? Theme.highlightColor : Theme.secondaryColor);
                    font.pixelSize: Theme.fontSizeSmall;
                    anchors.verticalCenter: parent.verticalCenter;
                }
            }
        }
        MouseArea {
            id: tabWatchList;
            anchors {
                top: parent.top;
                left: parent.horizontalCenter;
                right: parent.right;
                bottom: parent.bottom;
            }
            onClicked: { engine.sortedFilteredMoviesModel.toggleFilterSeen = false; }

            property bool active : !engine.sortedFilteredMoviesModel.toggleFilterSeen;

            Row {
                spacing: Theme.paddingSmall;
                anchors.centerIn: parent;


                Image {
                    width: height;
                    height: (Theme.itemSizeMedium * 0.5);
                    source: "image://theme/icon-l-date?%1".arg (lblWatchlist.color.toString ());
                    anchors.verticalCenter: parent.verticalCenter;
                }
                Label {
                    id: lblWatchlist;
                    text: qsTr ("To-watch");
                    color: (tabWatchList.active ? Theme.highlightColor : Theme.secondaryColor);
                    font.pixelSize: Theme.fontSizeSmall;
                    anchors.verticalCenter: parent.verticalCenter;
                }
            }
        }
    }
}
