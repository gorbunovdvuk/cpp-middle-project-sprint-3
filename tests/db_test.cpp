#include "book_database.hpp"
#include "filters.hpp"
#include "statistics.hpp"

#include <deque>
#include <gtest/gtest.h>
#include <vector>

using namespace bookdb;

TEST(StatisticsTest, BuildAuthorHistogramFlatTest) {
    using namespace std::string_view_literals;

    auto db = BookDatabase<>{
        {"", "A", 0, Genre::Unknown, 0.0, 0},
        {"", "B", 0, Genre::Unknown, 0.0, 0},
        {"", "A", 0, Genre::Unknown, 0.0, 0},
    };

    auto hist = buildAuthorHistogramFlat(db);
    EXPECT_EQ(hist.size(), 2);
    EXPECT_EQ(hist["A"], 2);
    EXPECT_EQ(hist["B"], 1);
}

TEST(StatisticsTest, CalculateGenreRatingsTest) {
    auto db = BookDatabase<>{
        {"", "", 0, Genre::SciFi, 2.0, 0},
        {"", "", 0, Genre::SciFi, 1.0, 0},
        {"", "", 0, Genre::Fiction, 3.0, 0},
    };
    auto genre_ratings = calculateGenreRatings(db);
    EXPECT_EQ(genre_ratings.size(), 2);
    EXPECT_DOUBLE_EQ(genre_ratings[Genre::SciFi], 1.5);
    EXPECT_DOUBLE_EQ(genre_ratings[Genre::Fiction], 3.0);
}

TEST(StatisticsTest, CalculateAverageRatingTest) {
    auto db = BookDatabase<>{
        {"", "", 0, Genre::Unknown, 1.0, 0},
        {"", "", 0, Genre::Unknown, 2.0, 0},
        {"", "", 0, Genre::Unknown, 5.0, 0},
    };
    auto average_rating = calculateAverageRating(db);
    EXPECT_DOUBLE_EQ(average_rating, 8.0 / 3);
}

TEST(StatisticsTest, EmptyCalculateAverageRatingTest) {
    EXPECT_DOUBLE_EQ(calculateAverageRating(BookDatabase<>{}), 0.0);
}

TEST(StatisticsTest, GetTopNBy) {
    BookDatabase<> books{{"The Caves of Steel", "Isaac Asimov", 1953, Genre::SciFi, 87, 120},
                         {"The Murder of Roger Ackroyd", "Agatha Christie", 1926, Genre::Mystery, 89, 220},
                         {"Death on the Nile", "Agatha Christie", 1937, Genre::Mystery, 88, 210},
                         {"1Q84", "Haruki Murakami", 2009, Genre::Fiction, 85, 150}};
    {
        auto result = getTopNBy(books, 2, comp::LessByAuthor{});
        EXPECT_EQ(result.size(), 2);
        EXPECT_EQ(std::format<const Book &>("{}", result[0]),
                  "{\"Agatha Christie\", \"The Murder of Roger Ackroyd\", 1926, Genre::Mystery, 89, 220}");
        EXPECT_EQ(std::format<const Book &>("{}", result[1]),
                  "{\"Agatha Christie\", \"Death on the Nile\", 1937, Genre::Mystery, 88, 210}");
    }
    {
        auto result = getTopNBy(books, 2, comp::GreaterByRating{});
        EXPECT_EQ(result.size(), 2);
        EXPECT_EQ(std::format<const Book &>("{}", result[0]),
                  "{\"Agatha Christie\", \"The Murder of Roger Ackroyd\", 1926, Genre::Mystery, 89, 220}");
        EXPECT_EQ(std::format<const Book &>("{}", result[1]),
                  "{\"Agatha Christie\", \"Death on the Nile\", 1937, Genre::Mystery, 88, 210}");
    }
}

TEST(StatisticsTest, SampleRandomBook) {
    BookDatabase<> books{
        {"Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 94, 300},
        {"Sense and Sensibility", "Jane Austen", 1811, Genre::Fiction, 88, 200},
        {"Foundation", "Isaac Asimov", 1951, Genre::SciFi, 92, 150},
        {"I, Robot", "Isaac Asimov", 1950, Genre::SciFi, 89, 140},
        {"The Caves of Steel", "Isaac Asimov", 1953, Genre::SciFi, 87, 120},
    };
    std::mt19937_64 rng{42};
    auto result = sampleRandomBooks(books, 2, rng);
    EXPECT_EQ(result.size(), 2);
}

constexpr BookDatabase<> bigBookDatabase() {
    return BookDatabase<>{{"Crime and Punishment", "Fyodor Dostoevsky", 1866, Genre::Fiction, 95, 250},
                          {"The Brothers Karamazov", "Fyodor Dostoevsky", 1880, Genre::Fiction, 96, 220},
                          {"Notes from Underground", "Fyodor Dostoevsky", 1864, Genre::Fiction, 90, 180},
                          {"Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 94, 300},
                          {"Sense and Sensibility", "Jane Austen", 1811, Genre::Fiction, 88, 200},
                          {"Foundation", "Isaac Asimov", 1951, Genre::SciFi, 92, 150},
                          {"I, Robot", "Isaac Asimov", 1950, Genre::SciFi, 89, 140},
                          {"The Caves of Steel", "Isaac Asimov", 1953, Genre::SciFi, 87, 120},
                          {"Murder on the Orient Express", "Agatha Christie", 1934, Genre::Mystery, 91, 250},
                          {"And Then There Were None", "Agatha Christie", 1939, Genre::Mystery, 94, 300},
                          {"The Murder of Roger Ackroyd", "Agatha Christie", 1926, Genre::Mystery, 89, 220},
                          {"Death on the Nile", "Agatha Christie", 1937, Genre::Mystery, 88, 210},
                          {"A Brief History of Time", "Stephen Hawking", 1988, Genre::NonFiction, 93, 340},
                          {"Harry Potter and the Philosopher’s Stone", "J.K. Rowling", 1997, Genre::Fiction, 95, 500},
                          {"Harry Potter and the Prisoner of Azkaban", "J.K. Rowling", 1999, Genre::Fiction, 94, 450},
                          {"Steve Jobs", "Walter Isaacson", 2011, Genre::Biography, 89, 160},
                          {"Norwegian Wood", "Haruki Murakami", 1987, Genre::Fiction, 88, 200},
                          {"1Q84", "Haruki Murakami", 2009, Genre::Fiction, 85, 150}};
}

TEST(StatisticsTest, SimpleFilterBooksTestAll) {
    auto books = bigBookDatabase();
    EXPECT_EQ(filterBooks(books.begin(), books.end(), [](auto &&) { return true; }).size(), books.size());
}

TEST(StatisticsTest, YearBetweenTest) {
    auto books = bigBookDatabase();
    auto result = filterBooks(books.begin(), books.end(), YearBetween(1987, 1988));
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(std::format<const Book &>("{}", result[0]),
              "{\"Stephen Hawking\", \"A Brief History of Time\", 1988, Genre::NonFiction, 93, 340}");
    EXPECT_EQ(std::format<const Book &>("{}", result[1]),
              "{\"Haruki Murakami\", \"Norwegian Wood\", 1987, Genre::Fiction, 88, 200}");
}

TEST(StatisticsTest, RatingAbove) {
    auto books = bigBookDatabase();
    auto result = filterBooks(books.begin(), books.end(), RatingAbove(95));
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(std::format<const Book &>("{}", result[0]),
              "{\"Fyodor Dostoevsky\", \"The Brothers Karamazov\", 1880, Genre::Fiction, 96, 220}");
}

TEST(StatisticsTest, AllOfTest) {
    auto books = bigBookDatabase();
    auto result = filterBooks(books.begin(), books.end(),
                              all_of(RatingAbove(95), AuthorIs("Fyodor Dostoevsky"), GenreIs(Genre::Fiction)));
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(std::format<const Book &>("{}", result[0]),
              "{\"Fyodor Dostoevsky\", \"The Brothers Karamazov\", 1880, Genre::Fiction, 96, 220}");
}

TEST(StatisticsTest, AnyOfTest) {
    auto books = bigBookDatabase();
    auto result = filterBooks(books.begin(), books.end(), any_of(RatingAbove(95), AuthorIs("Walter Isaacson")));
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(std::format<const Book &>("{}", result[0]),
              "{\"Fyodor Dostoevsky\", \"The Brothers Karamazov\", 1880, Genre::Fiction, 96, 220}");
    EXPECT_EQ(std::format<const Book &>("{}", result[1]),
              "{\"Walter Isaacson\", \"Steve Jobs\", 2011, Genre::Biography, 89, 160}");
}
