/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   include/graph_to_string.h
 * Description: graph stream operators
 *
 * Copyright (C) 2024 Dieter J Kybelksties <github@kybelksties.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * @date: 2024-10-05
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_GRAPH_TO_STRING_H_INCLUDED
#define NS_UTIL_GRAPH_TO_STRING_H_INCLUDED

#include "directed_graph.h"
#include "to_string.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <fstream>
#include <ostream>
#include <sstream>
#include <type_traits>

// Class to export a graph to a DOT file
template <typename Graph, typename Vertex, typename Edge>
class DotGraphExporter
{
  public:
    // Constructor
    DotGraphExporter(Graph const& graph)
        : graph_(graph)
    {
    }

    // Check if a type can be converted to a string
    template <typename T>
    std::string to_string(T const& value) const
    {
        if constexpr (std::is_convertible_v<T, std::string>)
        {
            return static_cast<std::string>(value);
        }
        else
        {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }
    }

    // Write the graph to a DOT file
    void writeToDotFile(std::string const& filename) const
    {
        std::ofstream outFile(filename);
        if (!outFile.is_open())
        {
            throw std::runtime_error("Could not open file to write DOT graph");
        }

        // Custom property writer for vertices and edges
        auto vertexWriter = [this](std::ostream& out, typename boost::graph_traits<Graph>::vertex_descriptor v) {
            out << "[label=\"" << to_string(graph_[v]) << "\"]";
        };

        auto edgeWriter = [this](std::ostream& out, typename boost::graph_traits<Graph>::edge_descriptor e) {
            out << "[label=\"" << to_string(graph_[e]) << "\"]";
        };

        // Write graph to the file using Boost's write_graphviz
        boost::write_graphviz(outFile, graph_, vertexWriter, edgeWriter);
    }

  private:
    Graph const& graph_; // Reference to the graph to export
};

#endif // NS_UTIL_GRAPH_TO_STRING_H_INCLUDED
