package ceng.ceng351.musicdb;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;

public class MUSICDB implements IMUSICDB {

    private static String user = "e2172138"; // TODO: Your userName
    private static String password = "502de3f5"; //  TODO: Your password
    private static String host = "144.122.71.57"; // host name
    private static String database = "db2172138"; // TODO: Your database name
    private static int port = 8084; // port

    private Connection con = null;

    /**
     * Place your initialization code inside if required.
     *
     * <p>
     * This function will be called before all other operations. If your implementation
     * need initialization , necessary operations should be done inside this function. For
     * example, you can set your connection to the database server inside this function.
     */
    @Override
    public void initialize() {

        String url = "jdbc:mysql://" + this.host + ":" + this.port + "/" + this.database;

        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            this.con =  DriverManager.getConnection(url, this.user, this.password);
        } catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        }

        return;
    }

    /**
     * Should create the necessary tables when called.
     *
     * @return the number of tables that are created successfully.
     */
    @Override
    public int createTables() {

        int result;
        int numberofTablesInserted = 0;

        String queryCreateUserTable = "CREATE TABLE IF NOT EXISTS user(" +
                                        "userID INT NOT NULL, " +
                                        "userName VARCHAR(60), " +
                                        "email VARCHAR(30), " +
                                        "password VARCHAR(30), " +
                                        "PRIMARY KEY (userID));";

        String queryCreateArtistTable = "CREATE TABLE IF NOT EXISTS artist(" +
                                            "artistID INT NOT NULL, " +
                                            "artistName VARCHAR(60), " +
                                            "PRIMARY KEY (artistID));";

        String queryCreateAlbumTable = "CREATE TABLE IF NOT EXISTS album(" +
                                        "albumID INT NOT NULL, " +
                                        "title VARCHAR(60), " +
                                        "albumGenre VARCHAR(30), " +
                                        "albumRating DOUBLE, " +
                                        "releaseDate DATE, " +
                                        "artistID INT, " +
                                        "FOREIGN KEY (artistID) " +
                                            "REFERENCES artist(artistID) " +
                                                "ON DELETE CASCADE " +
                                                "ON UPDATE CASCADE, " +
                                        "PRIMARY KEY (albumID));";

        String queryCreateSongTable = "CREATE TABLE IF NOT EXISTS song(" +
                                        "songID INT NOT NULL, " +
                                        "songName VARCHAR(60), " +
                                        "genre VARCHAR(30), " +
                                        "rating DOUBLE, " +
                                        "artistID INT, " +
                                        "albumID INT, " +
                                        "FOREIGN KEY (artistID) " +
                                            "REFERENCES artist(artistID) " +
                                                "ON DELETE CASCADE " +
                                                "ON UPDATE CASCADE, " +
                                        "FOREIGN KEY (albumID) " +
                                            "REFERENCES album(albumID) " +
                                                "ON DELETE CASCADE " +
                                                "ON UPDATE CASCADE, " +
                                        "PRIMARY KEY (songID));";

        String queryCreateListenTable = "CREATE TABLE IF NOT EXISTS listen(" +
                                            "userID INT NOT NULL, " +
                                            "songID INT NOT NULL, " +
                                            "lastListenTime TIMESTAMP, " +
                                            "listenCount INT, " +
                                            "FOREIGN KEY (userID) " +
                                                "REFERENCES user(userID) " +
                                                    "ON DELETE CASCADE " +
                                                    "ON UPDATE CASCADE, " +
                                            "FOREIGN KEY (songID) " +
                                                "REFERENCES song(songID) " +
                                                    "ON DELETE CASCADE " +
                                                    "ON UPDATE CASCADE, " +
                                            "PRIMARY KEY (userID, songID));";

        // Create User Table
        try {
            Statement statement = this.con.createStatement();

            statement.executeUpdate(queryCreateUserTable);
            numberofTablesInserted++;
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        // Create Artist Table
        try {
            Statement statement = this.con.createStatement();

            statement.executeUpdate(queryCreateArtistTable);
            numberofTablesInserted++;
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        // Create Album Table
        try {
            Statement statement = this.con.createStatement();

            statement.executeUpdate(queryCreateAlbumTable);
            numberofTablesInserted++;
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        // Create Song Table
        try {
            Statement statement = this.con.createStatement();

            statement.executeUpdate(queryCreateSongTable);
            numberofTablesInserted++;
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        // Create Listen Table
        try {
            Statement statement = this.con.createStatement();

            statement.executeUpdate(queryCreateListenTable);
            numberofTablesInserted++;
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return numberofTablesInserted;

    }

    /**
     * Should drop the tables if exists when called.
     *
     * @return the number of tables are dropped successfully.
     */
    @Override
    public int dropTables() {

        int result;
        int numberofTablesDropped = 0;

        String queryDropListenTable = "DROP TABLE IF EXISTS listen;";

        String queryDropSongTable = "DROP TABLE IF EXISTS song;";

        String queryDropAlbumTable = "DROP TABLE IF EXISTS album;";

        String queryDropArtistTable = "DROP TABLE IF EXISTS artist;";

        String queryDropUserTable = "DROP TABLE IF EXISTS user;";

        // Drop Listen Table
        try {
            Statement statement = this.con.createStatement();

            statement.executeUpdate(queryDropListenTable);
            numberofTablesDropped++;
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        // Drop Song Table
        try {
            Statement statement = this.con.createStatement();

            statement.executeUpdate(queryDropSongTable);
            numberofTablesDropped++;
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        // Drop Album Table
        try {
            Statement statement = this.con.createStatement();

            statement.executeUpdate(queryDropAlbumTable);
            numberofTablesDropped++;
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        // Drop Artist Table
        try {
            Statement statement = this.con.createStatement();

            statement.executeUpdate(queryDropArtistTable);
            numberofTablesDropped++;
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        // Drop User Table
        try {
            Statement statement = this.con.createStatement();

            statement.executeUpdate(queryDropUserTable);
            numberofTablesDropped++;
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return numberofTablesDropped;

    }

    /**
     * Should insert an array of Album into the database.
     *
     * @return Number of rows inserted successfully.
     */
    @Override
    public int insertAlbum(Album[] albums) {

        int numberofRowsInserted = 0;
        int arrayLength = albums.length;

        String queryInserttoAlbumTable = "INSERT INTO album(albumID, title, albumGenre, albumRating, releaseDate, artistID) VALUES (";
        String[] temp;

        for (int i = 0; i < arrayLength; i++) {

            temp = albums[i].toString().split("\t", 6);
            temp[1] = "\"" + temp[1] + "\"";
            temp[2] = "\"" + temp[2] + "\"";
            temp[4] = "\"" + temp[4] + "\"";

            //Insert Albums
            try {
                Statement statement = this.con.createStatement();

                statement.executeUpdate(queryInserttoAlbumTable + String.join(", ", temp) + ");");
                numberofRowsInserted++;
                statement.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

        }

        return numberofRowsInserted;

    }

    /**
     * Should insert an array of Artist into the database.
     *
     * @return Number of rows inserted successfully.
     */
    @Override
    public int insertArtist(Artist[] artists) {

        int numberofRowsInserted = 0;
        int arrayLength = artists.length;

        String queryInserttoArtistTable = "INSERT INTO artist(artistID, artistName) VALUES (";
        String[] temp;

        for (int i = 0; i < arrayLength; i++) {

            temp = artists[i].toString().split("\t", 2);
            temp[1] = "\"" + temp[1] + "\"";

            // Insert Artist
            try {
                Statement statement = this.con.createStatement();

                statement.executeUpdate(queryInserttoArtistTable + String.join(", ", temp) + ");");
                numberofRowsInserted++;
                statement.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

        }

        return numberofRowsInserted;

    }

    /**
     * Should insert an array of Song into the database.
     *
     * @return Number of rows inserted successfully.
     */
    @Override
    public int insertSong(Song[] songs) {

        int numberofRowsInserted = 0;
        int arrayLength = songs.length;

        String queryInserttoSongTable = "INSERT INTO song(songID, songName, genre, rating, artistID, albumID) VALUES (";
        String[] temp;

        for (int i = 0; i < arrayLength; i++) {

            temp = songs[i].toString().split("\t", 6);
            temp[1] = "\"" + temp[1] + "\"";
            temp[2] = "\"" + temp[2] + "\"";

            // Insert Songs
            try {
                Statement statement = this.con.createStatement();

                statement.executeUpdate(queryInserttoSongTable + String.join(", ", temp) + ");");
                numberofRowsInserted++;
                statement.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

        }

        return numberofRowsInserted;

    }

    /**
     * Should insert an array of User into the database.
     *
     * @return Number of rows inserted successfully.
     */
    @Override
    public int insertUser(User[] users) {

        int numberofRowsInserted = 0;
        int arrayLength = users.length;

        String queryInserttoUserTable = "INSERT INTO user(userID, userName, email, password) VALUES (";
        String[] temp;

        for (int i = 0; i < arrayLength; i++) {

            temp = users[i].toString().split("\t", 4);
            temp[1] = "\"" + temp[1] + "\"";
            temp[2] = "\"" + temp[2] + "\"";
            temp[3] = "\"" + temp[3] + "\"";

            // Insert Users
            try {
                Statement statement = this.con.createStatement();

                statement.executeUpdate(queryInserttoUserTable + String.join(", ", temp) + ");");
                numberofRowsInserted++;
                statement.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

        }

        return numberofRowsInserted;

    }

    /**
     * Should insert an array of Listen into the database.
     *
     * @return Number of rows inserted successfully.
     */
    @Override
    public int insertListen(Listen[] listens) {

        int numberofRowsInserted = 0;
        int arrayLength = listens.length;

        String queryInserttoListenTable = "INSERT INTO listen(userID, songID, lastListenTime, listenCount) VALUES (";
        String[] temp;

        for (int i = 0; i < arrayLength; i++) {

            temp = listens[i].toString().split("\t", 4);
            temp[2] = "\"" + temp[2] + "\"";

            // Insert Listens
            try {
                Statement statement = this.con.createStatement();

                statement.executeUpdate(queryInserttoListenTable + String.join(", ", temp) + ");");
                numberofRowsInserted++;
                statement.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

        }

        return numberofRowsInserted;

    }

    /**
     * Should return songs with the highest rating
     *
     * @return ArtistNameSongNameGenreRatingResult[]
     */
    @Override
    public QueryResult.ArtistNameSongNameGenreRatingResult[] getHighestRatedSongs() {

        int size;

        ResultSet rs;

        QueryResult.ArtistNameSongNameGenreRatingResult[] resultArray = null;
        ArrayList<QueryResult.ArtistNameSongNameGenreRatingResult> temp = new ArrayList<>();

        String query = "SELECT DISTINCT artist.artistName, song.songName, song.genre, song.rating " +
                        "FROM artist, song " +
                        "WHERE artist.artistID = song.artistID AND " +
                                "song.rating = (SELECT MAX(song.rating) FROM song) " +
                        "ORDER BY artist.artistName ASC;";

        try {

            Statement st = this.con.createStatement();
            rs = st.executeQuery(query);

            if (rs.next()) {
                do {
                    temp.add(new QueryResult.ArtistNameSongNameGenreRatingResult(rs.getString("artistName"),
                                                                                    rs.getString("songName"),
                                                                                    rs.getString("genre"),
                                                                                    rs.getDouble("rating")));
                } while (rs.next());
            }

            size = temp.size();
            resultArray = new QueryResult.ArtistNameSongNameGenreRatingResult[size];

            for (int i = 0; i < size; i++) {
                resultArray[i] = temp.get(i);
            }

            st.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return resultArray;

    }

    /**
     * Should return the most recent album for the given artistName
     *
     * @return TitleReleaseDateRatingResult
     */
    @Override
    public QueryResult.TitleReleaseDateRatingResult getMostRecentAlbum(String artistName) {

        ResultSet rs;

        QueryResult.TitleReleaseDateRatingResult result = null;

        String query = "SELECT DISTINCT album.title, album.releaseDate, album.albumRating " +
                        "FROM album, artist " +
                        "WHERE artist.artistName = \"" + artistName + "\" AND " +
                                "artist.artistID = album.artistID AND " +
                                "album.releaseDate = (SELECT MAX(album.releaseDate) " +
                                                        "FROM album, artist " +
                                                        "WHERE artist.artistName = \"" + artistName + "\" AND " +
                                                                "artist.artistID = album.artistID);\n";

        try {

            Statement st = this.con.createStatement();
            rs = st.executeQuery(query);

            if (rs.next()) {

                result = new QueryResult.TitleReleaseDateRatingResult(rs.getString("title"),
                                                                        rs.getString("releaseDate"),
                                                                        rs.getDouble("albumRating"));
            }

            st.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return result;

    }

    /**
     * Should return songs that are listened by both userName1 & userName2
     *
     * @return ArtistNameSongNameGenreRatingResult[]
     */
    @Override
    public QueryResult.ArtistNameSongNameGenreRatingResult[] getCommonSongs(String userName1, String userName2) {

        int size = 0;

        ResultSet rs;

        QueryResult.ArtistNameSongNameGenreRatingResult[] resultArray = null;
        ArrayList<QueryResult.ArtistNameSongNameGenreRatingResult> temp = new ArrayList<>();

        String query = "SELECT DISTINCT artist.artistName, song.songName, song.genre, song.rating " +
                        "FROM artist, song " +
                        "WHERE artist.artistID = song.artistID AND " +
                                "song.songID IN (SELECT song.songID " +
                                            "FROM song, user, listen " +
                                            "WHERE song.songID = listen.songID AND " +
                                                    "listen.userID = user.userID AND " +
                                            "user.userName = \"" + userName1 + "\") AND " +
                                "song.songID IN (SELECT song.songID " +
                                            "FROM song, user, listen " +
                                            "WHERE song.songID = listen.songID AND " +
                                                    "listen.userID = user.userID AND " +
                                            "user.userName = \"" + userName2 + "\") " +
                        "ORDER BY song.rating DESC;";

        try {

            Statement st = this.con.createStatement();
            rs = st.executeQuery(query);

            if (rs.next()) {
                do {
                    temp.add(new QueryResult.ArtistNameSongNameGenreRatingResult(rs.getString("artistName"),
                                                                                    rs.getString("songName"),
                                                                                    rs.getString("genre"),
                                                                                    rs.getDouble("rating")));
                } while (rs.next());
            }

            size = temp.size();
            resultArray = new QueryResult.ArtistNameSongNameGenreRatingResult[size];

            for (int i = 0; i < size; i++) {
                resultArray[i] = temp.get(i);
            }

            st.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return resultArray;

    }

    /**
     * Should return artists whose songs are listened by the given user
     * and number of times his/her songs have been listened by the given user
     *
     * @return ArtistNameNumberOfSongsResult[]
     */
    @Override
    public QueryResult.ArtistNameNumberOfSongsResult[] getNumberOfTimesSongsListenedByUser(String userName) {

        int size = 0;

        ResultSet rs;

        QueryResult.ArtistNameNumberOfSongsResult[] resultArray = null;
        ArrayList<QueryResult.ArtistNameNumberOfSongsResult> temp = new ArrayList<>();

        String query = "SELECT DISTINCT artist.artistName, SUM(listen.listenCount) AS listenSum " +
                        "FROM artist, listen, song, user " +
                        "WHERE user.userName = \"" + userName + "\" AND " +
                                "listen.userID = user.userID AND " +
                                "listen.songID = song.songID AND " +
                                "song.artistID = artist.artistID " +
                        "GROUP BY artist.artistName " +
                        "ORDER BY artist.artistName ASC;";

        try {

            Statement st = this.con.createStatement();
            rs = st.executeQuery(query);

            if (rs.next()) {
                do {
                    temp.add(new QueryResult.ArtistNameNumberOfSongsResult(rs.getString("artistName"),
                                                                            rs.getInt("listenSum")));
                } while (rs.next());
            }

            size = temp.size();
            resultArray = new QueryResult.ArtistNameNumberOfSongsResult[size];

            for (int i = 0; i < size; i++) {
                resultArray[i] = temp.get(i);
            }

            st.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return resultArray;

    }

    /**
     * Should return users who have listened all songs of a given artist
     *
     * @return User[]
     */
    @Override
    public User[] getUsersWhoListenedAllSongs(String artistName) {

        int size = 0;

        ResultSet rs;

        User[] resultArray = null;
        ArrayList<User> temp = new ArrayList<>();

        String query = "SELECT DISTINCT user.userID, user.userName, user.email, user.password " +
                        "FROM user " +
                        "WHERE NOT EXISTS (SELECT song.songID " +
                                            "FROM song, artist " +
                                            "WHERE artist.artistName = \"" + artistName + "\" AND " +
                                                    "song.artistID = artist.artistID AND " +
                                                    "song.songID NOT IN (SELECT listen.songID " +
                                                                        "FROM listen " +
                                                                        "WHERE listen.userID = user.userID)) " +
                        "ORDER BY user.userID ASC;";

        try {

            Statement st = this.con.createStatement();
            rs = st.executeQuery(query);

            if (rs.next()) {
                do {
                    temp.add(new User(rs.getInt("userID"),
                                        rs.getString("userName"),
                                        rs.getString("email"),
                                        rs.getString("password")));
                } while (rs.next());
            }

            size = temp.size();
            resultArray = new User[size];

            for (int i = 0; i < size; i++) {
                resultArray[i] = temp.get(i);
            }

            st.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return resultArray;


    }

    /**
     * Should return the userID, userName and number of songs listened by this user such that
     * none of these songs are listened by any other user.
     *
     * @return QueryResult.UserIdUserNameNumberOfSongsResult[]
     */
    @Override
    public QueryResult.UserIdUserNameNumberOfSongsResult[]  getUserIDUserNameNumberOfSongsNotListenedByAnyone() {

        int size = 0;

        ResultSet rs;

        QueryResult.UserIdUserNameNumberOfSongsResult[] resultArray = null;
        ArrayList<QueryResult.UserIdUserNameNumberOfSongsResult> temp = new ArrayList<>();

        String query = "SELECT DISTINCT user.userID, user.userName, COUNT(song.songID) AS songCount " +
                        "FROM user, song, listen " +
                        "WHERE user.userID = listen.userID AND " +
                                "listen.songID = song.songID AND " +
                                "song.songID NOT IN (SELECT song.songID " +
                                                    "FROM song, listen " +
                                                    "WHERE song.songID = listen.songID AND " +
                                                            "listen.userID <> user.userID) " +
                        "GROUP BY user.userID " +
                        "ORDER BY user.userID ASC;";

        try {

            Statement st = this.con.createStatement();
            rs = st.executeQuery(query);

            if (rs.next()) {
                do {
                    temp.add(new QueryResult.UserIdUserNameNumberOfSongsResult(rs.getInt("userID"),
                                                                                rs.getString("userName"),
                                                                                rs.getInt("songCount")));
                } while (rs.next());
            }

            size = temp.size();
            resultArray = new QueryResult.UserIdUserNameNumberOfSongsResult[size];

            for (int i = 0; i < size; i++) {
                resultArray[i] = temp.get(i);
            }

            st.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return resultArray;

    }

    /**
     * Should return artists who have sung pop music at least once and whose average rating of
     * all songs is greater than the given rating
     *
     * @return Artist[]
     */
    @Override
    public Artist[] getArtistSingingPopGreaterAverageRating(double rating) {

        int size = 0;

        ResultSet rs;

        Artist[] resultArray = null;
        ArrayList<Artist> temp = new ArrayList<>();

        String query = "SELECT DISTINCT artist.artistID, artist.artistName " +
                        "FROM artist, song " +
                        "WHERE artist.artistID = song.artistID " +
                        "GROUP BY artist.artistID " +
                        "HAVING AVG(song.rating) > " + rating + " AND \"pop\" IN (SELECT temp_song.genre " +
                                                                                    "FROM song AS temp_song " +
                                                                                    "WHERE artist.artistID = temp_song.artistID) " +
                        "ORDER BY artist.artistID ASC;";

        try {

            Statement st = this.con.createStatement();
            rs = st.executeQuery(query);

            if (rs.next()) {
                do {
                    temp.add(new Artist(rs.getInt("artistID"),
                                        rs.getString("artistName")));
                } while (rs.next());
            }

            size = temp.size();
            resultArray = new Artist[size];

            for (int i = 0; i < size; i++) {
                resultArray[i] = temp.get(i);
            }

            st.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return resultArray;

    }

    /**
     * Retrieve songs with the lowest rating in pop category, in case of more than one song exist,
     * retrieve the least listened ones
     *
     * @return Song[]
     */
    @Override
    public Song[] retrieveLowestRatedAndLeastNumberOfListenedSongs() {

        int size = 0;

        ResultSet rs;

        Song[] resultArray = null;
        ArrayList<Song> temp = new ArrayList<>();

        String query = "SELECT DISTINCT pswlr.songID, pswlr.songName, pswlr.rating, pswlr.genre, pswlr.artistID, pswlr.albumID " +
                        "FROM (SELECT song.songID, song.songName, song.rating, song.genre, song.artistID, song.albumID " +
                                "FROM song " +
                                "WHERE song.genre = \"pop\" AND " +
                                        "song.rating = (SELECT MIN(song.rating) " +
                                                        "FROM song " +
                                                        "WHERE song.genre = \"pop\")) AS pswlr, listen " +
                        "WHERE pswlr.songID = listen.songID " +
                        "GROUP BY pswlr.songID " +
                        "HAVING SUM(listen.listenCount) = (SELECT MIN(temp.s) " +
                                                            "FROM (SELECT SUM(listen.listenCount) AS s " +
                                                                    "FROM listen, song " +
                                                                    "WHERE listen.songID = song.songID AND " +
                                                                            "song.genre = \"pop\" AND " +
                                                                            "song.rating = (SELECT MIN(song.rating) " +
                                                                                            "FROM song " +
                                                                                            "WHERE song.genre = \"pop\") " +
                                                                    "GROUP BY song.songID) AS temp) " +
                        "ORDER BY pswlr.songID ASC;";

        try {

            Statement st = this.con.createStatement();
            rs = st.executeQuery(query);

            if (rs.next()) {
                do {
                    temp.add(new Song(rs.getInt("songID"),
                                        rs.getString("songName"),
                                        rs.getString("genre"),
                                        rs.getDouble("rating"),
                                        rs.getInt("artistID"),
                                        rs.getInt("albumID")));
                } while (rs.next());
            }

            size = temp.size();
            resultArray = new Song[size];

            for (int i = 0; i < size; i++) {
                resultArray[i] = temp.get(i);
            }

            st.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return resultArray;

    }

    /**
     * Multiply the rating of the album by 1.5 whose releaseDate is after for a given date
     *
     * @return the row count for SQL Data Manipulation Language (DML) statements
     */
    @Override
    public int multiplyRatingOfAlbum(String releaseDate) {

        int result = 0;

        String query = "UPDATE album SET albumRating = albumRating * 1.5 WHERE releaseDate > \"" + releaseDate + "\";";

        try {
            Statement statement = this.con.createStatement();

            result = statement.executeUpdate(query);
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return result;

    }

    /**
     * Delete the song for the given songName
     *
     * @return Song
     */
    @Override
    public Song deleteSong(String songName) {

        ResultSet rs;

        String getQueryforSong = "SELECT DISTINCT song.songID, song.songName, song.genre, song.rating, song.artistID, song.albumID FROM song WHERE song.songName = \"" + songName + "\";";

        Song result = null;

        String deleteQueryforSong = "DELETE FROM song WHERE songName = \"" + songName + "\";";

        try {

            Statement st = this.con.createStatement();
            rs = st.executeQuery(getQueryforSong);

            if (rs.next()) {
                result = new Song(rs.getInt("songID"),
                                    rs.getString("songName"),
                                    rs.getString("genre"),
                                    rs.getDouble("rating"),
                                    rs.getInt("artistID"),
                                    rs.getInt("albumID"));
            }

            st.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        try {
            Statement statement = this.con.createStatement();

            statement.executeUpdate(deleteQueryforSong);
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return result;

    }

}

