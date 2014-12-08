// vertex_source.hpp
#pragma once

#include <agg_basics.h>

namespace agg {

class test_vertex_source {
	double const *m_polygon;
	unsigned m_num_points;
	unsigned m_vertex;
	bool     m_roundoff;
	bool     m_close;
public:
	test_vertex_source(double const* polygon, unsigned np,
		bool roundoff = false, bool close = true) : m_polygon(polygon),
		m_num_points(np),
		m_vertex(0),
		m_roundoff(roundoff),
		m_close(close)
	{}
	
	void close(bool f) { m_close = f;    }
	bool close() const { return m_close; }
	
	void rewind(unsigned)
	{
		m_vertex = 0;
	}

	unsigned vertex(double* x, double* y)
	{
		if(m_vertex > m_num_points) return path_cmd_stop;
		if(m_vertex == m_num_points) 
		{
			++m_vertex;
			return path_cmd_end_poly | (m_close ? path_flags_close : 0);
		}
		*x = m_polygon[m_vertex * 2];
		*y = m_polygon[m_vertex * 2 + 1];
		if(m_roundoff)
		{
			*x = floor(*x) + 0.5;
			*y = floor(*y) + 0.5;
		}
		++m_vertex;
		return (m_vertex == 1) ? path_cmd_move_to : path_cmd_line_to;
	}
};

}
