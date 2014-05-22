import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: page

    SilicaGridView {
        id: view;
        cellWidth: (width * 0.25);
        cellHeight: (coverHeight * cellWidth / coverWidth);
        model: engine.moviesModel;
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


    }
}
