#include <algorithm>
#include <set>
#include <stdexcept>
#include <vector>

using my_time_t = long;
using car_info_t = std::pair<my_time_t, my_time_t>; // pair<arrival, departure>
using plot_point_t = std::pair<my_time_t, long>; // pair<at, value>
using plot_t = std::vector<plot_point_t>;
using busiest_times_t = std::vector<std::pair<my_time_t, my_time_t>>; // list of intervals

void add_plot_point(plot_t& plot, my_time_t at, long value) {
    if (!plot.empty()) {
        // skip value duplicates
        if (plot.rbegin()->second == value)
            return;
        // skip unnecessary points with same values
        if (plot.rbegin()->first == at) {
            plot.rbegin()->second = value;
            if (plot.size() >= 2) {
                auto it = plot.end();
                std::advance(it, -2);
                if (it->second == value)
                    plot.erase(++it);
                return;
            }
        }
    }
    plot.emplace_back(at, value);
}

// Complexity : approx. O(n*log(n)), abs<(n*log(n))^2 + n, n = data.size()
// Additional memory consumption: approx. O(n), abs<3n
// Memory reallocations can be reduced by replacement std::vector to std::list for plot_t and busiest_times_t
// Note: In order to make plot output easily removed, I don't compute busiest_times_t by plot data, although it's possible and more simple.
busiest_times_t process(std::vector<car_info_t>& data, plot_t& plot) {
    // data: order by data.first asc, data.second desc
    std::sort(data.begin(), data.end(),
        [](const car_info_t& a, const car_info_t& b) -> bool {
            if (a.first < b.first)
                return true;
            else if (a.first == b.first)
                return a.second >= b.second;
            else
                return false;
        }
    );

    busiest_times_t result;
    long counter = 0;
    long max_load = 0;
    bool flag_new_result_interval = true;
    std::multiset<my_time_t> departure_times;   // sorted set
    for (const car_info_t& info : data) {
        // sanity check: ignore cars with equal arrival and departure times
        if (info.first == info.second)
            continue;
        else if (info.first > info.second)
            throw std::runtime_error("Invalid parking interval");

        // cars departures
        for (auto it = departure_times.begin(); it != departure_times.end(); it = departure_times.erase(it)) {
            if (*it > info.first)
                break;
            if (counter == max_load && !flag_new_result_interval && *it != info.first) {
                // There's a gap in time between car departure and new car arrival.
                // Modify the result intervals accordingly.
                flag_new_result_interval = true;
                result.rbegin()->second = *it;
            }
            counter--;
            add_plot_point(plot, *it, counter);
        }

        // New car arrived
        counter++;
        departure_times.insert(info.second);

        // Add point for plot
        add_plot_point(plot, info.first, counter);

        // Compute result intervals
        if (counter > max_load) {
            // Higher load: new result vector
            max_load = counter;
            result.clear();
            result.emplace_back(info.first, -1 /* placeholder, unused */);
            flag_new_result_interval = false;
        }
        else if (counter == max_load) {
            // Same load: modify end of last interval according to a flag
            if (flag_new_result_interval) {
                result.emplace_back(info.first, -1 /* placeholder, unused */);
                flag_new_result_interval = false;
            }
            else {
                //result.rbegin()->second = info.first; // unused, optimized out
            }
        }
        else {
            // Lower load: add new interval next time
            flag_new_result_interval = true;
        }
    }
    if (counter == max_load && !flag_new_result_interval && !result.empty() && !departure_times.empty())
        result.rbegin()->second = *departure_times.begin();
    for (my_time_t dt : departure_times) {
        counter--;
        add_plot_point(plot, dt, counter);
    }
    return result;
}





#ifndef USE_QT

// Basic console implementation

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

my_time_t parse_time(const std::string& s) {
    if (s.size() >= 3) {
        char *end;
        const char *start = s.c_str();
        long hour = std::strtol(start, &end, 10);
        if (start == end || *end != ':')
            goto error;
        start = end + 1;
        long minute = std::strtol(start, &end, 10);
        if (start == end || *end != 0)
            goto error;
        if (minute >= 60 || minute < 0 || hour < 0)
            goto error;
        return hour * 100 + minute;
    }
error:
    std::stringstream stream;
    stream << "Invalid time format: " << s;
    throw std::invalid_argument(stream.str().c_str());
}

int main(int argc, char* argv[]) {
    std::cerr << "Usage: \n"
        "ParkingLot.exe <input filename>\n\n";

    if (argc != 2) {
        std::cerr << "Invalid number of arguments" << std::endl;
        return 1;
    }

    try {
        // Handle input
        std::vector<car_info_t> data;
        std::ifstream in(argv[1]);
        std::string temp;
        for (;;) {
            my_time_t arrival, departure;
            if (!(in >> temp))
                break;
            arrival = parse_time(temp);
            if (!(in >> temp))
                throw std::invalid_argument("missing departure time");
            departure = parse_time(temp);
            data.emplace_back(arrival, departure);
        }

        plot_t plot;
        busiest_times_t result = process(data, plot);
        std::cout << "Busiest time:" << std::endl;
        std::cout << std::setfill('0');
        for (const auto& period : result) {
            std::cout
                << std::setw(2) << (period.first / 100) << ':' << std::setw(2) << (period.first % 100) << " -\t"
                << std::setw(2) << (period.second / 100) << ':' << std::setw(2) << (period.second % 100) << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Plot data: " << std::endl;
        for (const plot_point_t& pt : plot) {
            std::cout
                << std::setw(2) << (pt.first / 100) << ':' << std::setw(2) << (pt.first % 100) << '\t'
                << pt.second << std::endl;
        }
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 2;
    }
}

#else

// For JSON parsing and visualization I've decided to use Qt

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

inline QDateTime fromInternalDateFormat(my_time_t dt) {
    return QDateTime::fromSecsSinceEpoch(static_cast<qint64>(dt) * 60);
}

inline my_time_t toInternalDateFormat(QDateTime dt) {
    return static_cast<long>(dt.toSecsSinceEpoch() / 60);
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    if (argc <= 1) {
        qCritical() << QObject::tr("Invalid number of arguments");
        return 1;
    }

    QFile file;
    file.setFileName(argv[1]);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << QObject::tr("Can't open %1 file").arg(argv[1]);
        return 2;
    }

    QString val = file.readAll();
    file.close();

    QJsonDocument jDoc = QJsonDocument::fromJson(val.toUtf8());
    QJsonArray times = jDoc.array();
    QSet<int> ids;
    std::vector<car_info_t> data;
    for (int i = 0; i < times.size(); ++i) {
        QJsonObject carInfo = times[i].toObject();
        int id = carInfo["Id"].toInt();
        if (ids.contains(id)) {
            qCritical() << QObject::tr("Car Id=%1 id duplication").arg(id);
            return 2;
        }
        ids.insert(id);
        QString temp = carInfo["ArrivalTime"].toString();
        QDateTime arrival = QDateTime::fromString(temp, Qt::DateFormat::ISODate);
        if (!arrival.isValid()) {
            qCritical() << QObject::tr("Invalid datetime %1").arg(temp);
            return 2;
        }
        temp = carInfo["LeaveTime"].toString();
        QDateTime departure = QDateTime::fromString(temp, Qt::DateFormat::ISODate);
        if (!departure.isValid()) {
            qCritical() << QObject::tr("Invalid datetime %1").arg(temp);
            return 2;
        }
        data.emplace_back(toInternalDateFormat(arrival), toInternalDateFormat(departure));
    }

    plot_t plot;
    busiest_times_t result = process(data, plot);

    QStringList periodsList;
    std::transform(
        result.begin(), result.end(), std::back_inserter(periodsList),
        [](const auto& period) {
            return QObject::tr("from %1 to %2").arg(
                fromInternalDateFormat(period.first).toString(Qt::DateFormat::ISODate),
                fromInternalDateFormat(period.second).toString(Qt::DateFormat::ISODate)
            );
        }
    );
    QTextStream(stdout) << QObject::tr("Busiest hours:") << "\n" << periodsList.join("\n") << "\n\n";

    QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
    long prev_value = 0;
    for (const plot_point_t& pt: plot) {
        auto idx = fromInternalDateFormat(pt.first).toMSecsSinceEpoch();
        series->append(idx-1, prev_value);
        series->append(idx, pt.second);
        prev_value = pt.second;
    }

    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    //chart->setTitle(title);

    QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Parking load");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(820, 600);
    window.show();

    return a.exec();
}

#endif
