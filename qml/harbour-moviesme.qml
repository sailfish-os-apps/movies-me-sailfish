import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.moviesme.myPrivateImports 1.0
import "pages"
import "cover"

ApplicationWindow {
    cover: coverPage;
    initialPage: moviesListPage;

    readonly property int coverWidth  : 342;
    readonly property int coverHeight : 485;

    MoviesEngine {
        id: engine;
    }
    Component {
        id: coverPage;

        CoverPage { }
    }
    Component {
        id: moviesListPage;

        MoviesListPage { }
    }
    Component {
        id: movieDetailsPage;

        MovieDetailsPage { }
    }
    Component {
        id: moviesSearchAndAddPage;

        MoviesSearchAndAddPage { }
    }
}
