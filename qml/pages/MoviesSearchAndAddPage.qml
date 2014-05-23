import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.moviesme.myPrivateImports 1.0
import "../components"

Page {
    id: page;

    SilicaFlickable {
        id: view;
        contentHeight: (column.height + column.anchors.margins * 2);
        anchors.fill: parent;

        Column {
            id: column;
            width: view.width;
            spacing: Theme.paddingMedium;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }

            PageHeader {
                title: qsTr ("Add a new movie");
            }
            SearchField {
                id: inputSearch;
                label: placeholderText;
                placeholderText: qsTr ("Search by name");
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                Keys.onReturnPressed: {
                    focus = false;
                    engine.requestSearch (text);
                }

            }
            Grid {
                id: grid;
                columns: 2;
                spacing: Theme.paddingSmall;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    margins: Theme.paddingSmall;
                }

                property real itemWidth : (width - spacing) * 0.5;

                Repeater {
                    model: engine.searchModel;
                    delegate: ListItem {
                        id: itemSerie;
                        width: grid.itemWidth;
                        contentHeight: (layout.height + layout.anchors.margins * 2);
                        onClicked: {
                            engine.requestFullMovieInfo (model ["tmdbId"]);
                            pageStack.navigateBack ();
                        }

                        Rectangle {
                            color: "white";
                            opacity: 0.15;
                            anchors {
                                fill: parent;
                                margins: Theme.paddingSmall;
                            }
                        }
                        Column {
                            id: layout;
                            spacing: anchors.margins;
                            anchors {
                                top: parent.top;
                                left: parent.left;
                                right: parent.right;
                                margins: Theme.paddingMedium;
                            }

                            Label {
                                text: model ["title"];
                                elide: Text.ElideRight;
                                font.pixelSize: Theme.fontSizeExtraSmall;
                                maximumLineCount: 2;
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                                anchors {
                                    left: parent.left;
                                    right: parent.right;
                                }
                            }
                            Image {
                                id: imgBanner;
                                source: model ["cover"];
                                opacity: (itemSerie.highlighted ? 0.85 : 1.0);
                                fillMode: Image.Stretch;
                                asynchronous: true;
                                height: (coverHeight * width / coverWidth);
                                anchors {
                                    left: parent.left;
                                    right: parent.right;
                                }

                                BusyIndicator {
                                    id: indicator;
                                    visible: running;
                                    running: (imgBanner.status !== Image.Ready);
                                    size: BusyIndicatorSize.Medium;
                                    anchors.centerIn: parent;
                                }
                            }
                        }
                    }
                }
            }
            Label {
                text: (view.count
                       ? qsTr ("%1 results").arg (view.count)
                       : (inputSearch.text !== ""
                          ? qsTr ("No matching result !")
                          : ""));
                width: view.width;
                horizontalAlignment: Text.AlignHCenter;
            }
        }

        VerticalScrollDecorator {}
    }
}
