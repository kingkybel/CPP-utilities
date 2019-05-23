/*
 * File Name:   traceutil.h
 * Description: debug tracing
 *
 * Copyright (C) 2019 Dieter J Kybelksties
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
 * @date: 2019-05-23
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_TRACEUTIL_H_INCLUDED
#define	NS_UTIL_TRACEUTIL_H_INCLUDED


#if defined DO_TRACE_

#define TRACE0 { std::cout << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << std::endl; }
#define TRACE1(v1) { std::cout << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << " " << #v1 << "=" << v1 << std::endl; }
// multi process versions for forked processes
#define PTRACE0 { std::cout << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << " PID=" << getpid() << std::endl; }
#define PTRACE1(v1) { std::cout << __FILE__ << ":" << __LINE__  << " " << __FUNCTION__ << " PID=" << getpid() << " " << #v1 << "=" << v1 << std::endl; }

#else

#define TRACE0
#define TRACE1(v1)
// multi process versions for forked processes
#define PTRACE0
#define PTRACE1(v1)

#endif // DO_TRACE_


#endif	// NS_UTIL_TRACEUTIL_H_INCLUDED

