#ifndef _UTILITIES_H_qweqweqweqwe_
#define _UTILITIES_H_qweqweqweqwe_

#include <boost/chrono/chrono_io.hpp>
#include <ostream>
#include <iostream>

namespace boost {
namespace chrono {

	typedef boost::chrono::duration<long long, boost::ratio<86400> > days;
	typedef boost::chrono::duration<long long, boost::centi> centiseconds;

	// format duration as [-]d hh::mm::ss.cc
	// ----------------------------------------------------------------------------------------------
	template <class CharT, class Traits, class Rep, class Period>
	std::basic_ostream<CharT, Traits>& duration_format_daytime(std::basic_ostream<CharT, Traits>& os, boost::chrono::duration<Rep, Period> d)
	// -----------------------------------------------------------------------------------------------
	{
		using namespace std;
		using namespace boost;

		// if negative, print negative sign and negate
		if (d < boost::chrono::duration<Rep, Period>(0))
		{
			d = -d;
			os << '-';
		}

		// round d to nearest centiseconds, to even on tie
		centiseconds cs = boost::chrono::duration_cast<centiseconds>(d);
		if (d - cs > boost::chrono::milliseconds(5) || (d - cs == boost::chrono::milliseconds(5) && cs.count() & 1))
			++cs;

		// separate seconds from centiseconds
		boost::chrono::seconds s = boost::chrono::duration_cast<boost::chrono::seconds>(cs);
		cs -= s;

		// separate minutes from seconds
		boost::chrono::minutes m = boost::chrono::duration_cast<boost::chrono::minutes>(s);
		s -= m;

		// separate hours from minutes
		boost::chrono::hours h = boost::chrono::duration_cast<boost::chrono::hours>(m);
		m -= h;

		// separate days from hours
		days dy = boost::chrono::duration_cast<days>(h);
		h -= dy;

		// print d hh:mm:ss.cc
		os << dy.count() << ' ';
		
		if (h < boost::chrono::hours(10))
			os << '0';
		os << h.count() << ':';
		
		if (m < boost::chrono::minutes(10))
			os << '0';
		os << m.count() << ':';

		if (s < boost::chrono::seconds(10))
			os << '0';
		os << s.count() << '.';

		if (cs < boost::chrono::centiseconds(10))
			os << '0';
		os << cs.count();

		return os;
	};

}
}

#endif
