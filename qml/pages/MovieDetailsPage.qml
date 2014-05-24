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
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }

            Item {
                width: height;
                height: (Theme.paddingLarge - parent.spacing);
            }
            Label {
                text: (engine.currentMovieObject ? engine.currentMovieObject.title : "");
                color: Theme.highlightColor;
                horizontalAlignment: Text.AlignRight;
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                font {
                    family: Theme.fontFamilyHeading;
                    pixelSize: Theme.fontSizeLarge;
                }
                anchors {
                    left: parent.left;
                    right: parent.right;
                    leftMargin: Theme.itemSizeLarge;
                    rightMargin: Theme.paddingMedium;
                }
            }
            Item {
                width: height;
                height: Theme.paddingLarge;
            }
            TextSwitch {
                id: switchWatched;
                text: qsTr ("Watched");
                description: "Whether you have seen this movie.";
                automaticCheck: false;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    margins: Theme.paddingMedium;
                }
                onDownChanged: {
                    var watched = !checked;
                    engine.requestUpdateVote (engine.currentMovieObject.tmdbId, (watched ? 0 : -1));
                }

                Binding on checked {
                    value: (engine.currentMovieObject && engine.currentMovieObject.vote >= 0);
                }
            }
            Row {
                spacing: 2;
                visible: switchWatched.checked;
                anchors.horizontalCenter: parent.horizontalCenter;

                Repeater {
                    model: 10;
                    delegate: Image {
                        width: (Theme.itemSizeSmall * 0.5);
                        height: width;
                        source: "image://theme/icon-l-%1?%2".arg (active ? "favorite" : "star").arg (fore.toString ());
                        anchors.verticalCenter: parent.verticalCenter;

                        property int   star   : (model.index +1);
                        property bool  active : (engine.currentMovieObject && star <= engine.currentMovieObject.vote);
                        property color fore   : (active ? Theme.highlightColor : Theme.secondaryColor);

                        MouseArea {
                            anchors {
                                fill: parent;
                                margins: -2;
                            }
                            onClicked: { engine.requestUpdateVote (engine.currentMovieObject.tmdbId, parent.star); }
                        }
                    }
                }
                Label {
                    text: (engine.currentMovieObject && engine.currentMovieObject.vote > 0
                           ? "%1/10".arg (engine.currentMovieObject.vote)
                           : qsTr ("N/A"));
                    color: Theme.primaryColor;
                    font.pixelSize: Theme.fontSizeSmall;
                    anchors.verticalCenter: parent.verticalCenter;
                }
            }
            Item {
                width: height;
                height: Theme.paddingLarge;
            }
            Label {
                text: (engine.currentMovieObject ? engine.currentMovieObject.summary : "");
                horizontalAlignment: Text.AlignJustify;
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    margins: Theme.paddingMedium;
                }
            }
            Item {
                width: height;
                height: Theme.paddingLarge;
            }
            BackgroundItem {
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                onClicked: { layoutReleaseDates.visible = !layoutReleaseDates.visible; }

                Label {
                    text: qsTr ("Release dates by country");
                    font.pixelSize: Theme.fontSizeMedium;
                    color: (layoutReleaseDates.visible ? Theme.highlightColor : Theme.secondaryHighlightColor);
                    anchors {
                        left: parent.left;
                        margins: Theme.paddingMedium;
                        verticalCenter: parent.verticalCenter;
                    }
                }
            }
            Rectangle {
                color: Theme.secondaryHighlightColor;
                height: 1;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
            }
            Column {
                id: layoutReleaseDates;
                visible: false;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                Repeater {
                    model: engine.releaseDatesList;
                    delegate: Item {
                        height: (Theme.itemSizeSmall * 0.5);
                        anchors {
                            left: layoutReleaseDates.left;
                            right: layoutReleaseDates.right;
                        }

                        Rectangle {
                            color: (model.index % 2 ? "white" : "black");
                            opacity: 0.05;
                            anchors.fill: parent;
                        }
                        Label {
                            text: engine.getFullCountryName (model.qtVariant ['iso_3166_1']);
                            color: Theme.primaryColor;
                            font.pixelSize: Theme.fontSizeSmall;
                            anchors {
                                left: parent.left;
                                margins: Theme.paddingMedium;
                                verticalCenter: parent.verticalCenter;
                            }
                        }
                        // TODO : add certification badge
                        Label {
                            text: Qt.formatDate (new Date (model.qtVariant ['release_date']));
                            color: Theme.secondaryColor;
                            font.pixelSize: Theme.fontSizeExtraSmall;
                            anchors {
                                right: parent.right;
                                margins: Theme.paddingMedium;
                                verticalCenter: parent.verticalCenter;
                            }
                        }
                    }
                }
                Item {
                    width: height;
                    height: Theme.paddingLarge;
                }
            }
            BackgroundItem {
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                onClicked: { layoutAltTitles.visible = !layoutAltTitles.visible; }

                Label {
                    text: qsTr ("Alternative titles");
                    font.pixelSize: Theme.fontSizeMedium;
                    color: (layoutAltTitles.visible ? Theme.highlightColor : Theme.secondaryHighlightColor);
                    anchors {
                        left: parent.left;
                        margins: Theme.paddingMedium;
                        verticalCenter: parent.verticalCenter;
                    }
                }
            }
            Rectangle {
                color: Theme.secondaryHighlightColor;
                height: 1;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
            }
            Column {
                id: layoutAltTitles;
                visible: false;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                Repeater {
                    model: engine.altTitlesList;
                    delegate: Item {
                        height: (layout.height + Theme.paddingLarge);
                        anchors {
                            left: layoutAltTitles.left;
                            right: layoutAltTitles.right;
                        }

                        Rectangle {
                            color: (model.index % 2 ? "white" : "black");
                            opacity: 0.05;
                            anchors.fill: parent;
                        }
                        Column {
                            id: layout;
                            anchors {
                                left: parent.left;
                                right: parent.right;
                                margins: Theme.paddingMedium;
                                verticalCenter: parent.verticalCenter;
                            }

                            Label {
                                text: engine.getFullCountryName (model.qtVariant ['iso_3166_1']);
                                color: Theme.primaryColor;
                                font.pixelSize: Theme.fontSizeExtraSmall;
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                                anchors {
                                    left: parent.left;
                                    right: parent.right;
                                }
                            }
                            Label {
                                text: model.qtVariant ['title'];
                                color: Theme.secondaryColor;
                                font.pixelSize: Theme.fontSizeSmall;
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                                anchors {
                                    left: parent.left;
                                    right: parent.right;
                                }
                            }
                        }
                    }
                }
                Item {
                    width: height;
                    height: Theme.paddingLarge;
                }
            }
            BackgroundItem {
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                onClicked: { layoutCastRoles.visible = !layoutCastRoles.visible; }

                Label {
                    text: qsTr ("Casting roles");
                    font.pixelSize: Theme.fontSizeMedium;
                    color: (layoutCastRoles.visible ? Theme.highlightColor : Theme.secondaryHighlightColor);
                    anchors {
                        left: parent.left;
                        margins: Theme.paddingMedium;
                        verticalCenter: parent.verticalCenter;
                    }
                }
            }
            Rectangle {
                color: Theme.secondaryHighlightColor;
                height: 1;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
            }
            Column {
                id: layoutCastRoles;
                visible: false;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                Repeater {
                    model: engine.castRolesList;
                    delegate: Item {
                        height: (Theme.itemSizeSmall * 0.5);
                        anchors {
                            left: layoutCastRoles.left;
                            right: layoutCastRoles.right;
                        }

                        Rectangle {
                            color: (model.index % 2 ? "white" : "black");
                            opacity: 0.05;
                            anchors.fill: parent;
                        }
                        Label {
                            text: model.qtVariant ['name'];
                            color: Theme.primaryColor;
                            font.pixelSize: Theme.fontSizeSmall;
                            anchors {
                                left: parent.left;
                                margins: Theme.paddingMedium;
                                verticalCenter: parent.verticalCenter;
                            }
                        }
                        Label {
                            text: model.qtVariant ['character'];
                            color: Theme.secondaryColor;
                            font.pixelSize: Theme.fontSizeExtraSmall;
                            anchors {
                                right: parent.right;
                                margins: Theme.paddingMedium;
                                verticalCenter: parent.verticalCenter;
                            }
                        }
                    }
                }
                Item {
                    width: height;
                    height: Theme.paddingLarge;
                }
            }
            BackgroundItem {
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                onClicked: { layoutCrewTeam.visible = !layoutCrewTeam.visible; }

                Label {
                    text: qsTr ("Crew team members");
                    font.pixelSize: Theme.fontSizeMedium;
                    color: (layoutCrewTeam.visible ? Theme.highlightColor : Theme.secondaryHighlightColor);
                    anchors {
                        left: parent.left;
                        margins: Theme.paddingMedium;
                        verticalCenter: parent.verticalCenter;
                    }
                }
            }
            Rectangle {
                color: Theme.secondaryHighlightColor;
                height: 1;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
            }
            Column {
                id: layoutCrewTeam;
                visible: false;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                Repeater {
                    model: engine.crewTeamList;
                    delegate: Item {
                        height: (Theme.itemSizeSmall * 0.5);
                        anchors {
                            left: layoutCrewTeam.left;
                            right: layoutCrewTeam.right;
                        }

                        Rectangle {
                            color: (model.index % 2 ? "white" : "black");
                            opacity: 0.05;
                            anchors.fill: parent;
                        }
                        Label {
                            text: model.qtVariant ['name'];
                            color: Theme.primaryColor;
                            font.pixelSize: Theme.fontSizeSmall;
                            anchors {
                                left: parent.left;
                                margins: Theme.paddingMedium;
                                verticalCenter: parent.verticalCenter;
                            }
                        }
                        Label {
                            text: model.qtVariant ['job'];
                            color: Theme.secondaryColor;
                            font.pixelSize: Theme.fontSizeExtraSmall;
                            anchors {
                                right: parent.right;
                                margins: Theme.paddingMedium;
                                verticalCenter: parent.verticalCenter;
                            }
                        }
                    }
                }
            }
        }
        VerticalScrollDecorator {}
    }
}
