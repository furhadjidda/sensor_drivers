/*
 *   This file is part of embedded software pico playground project.
 *
 *   embedded software pico playground projec is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   embedded software pico playground project is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License v3.0
 *   along with embedded software pico playground project.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef CONSTANTS_HPP_
#define CONSTANTS_HPP_

namespace AdafruitGpsConstants {

// used by check() for validity tests, room for future expansion
///< valid source ids
const char *sources[7] = {"II", "WI", "GP", "PG", "GN", "P", "ZZZ"};
#ifdef NMEA_EXTENSIONS
///< parseable sentence ids
const char *sentences_parsed[21] = {"GGA", "GLL", "GSA", "RMC", "DBT", "HDM", "HDT", "MDA", "MTW", "MWV",
                                    "RMB", "TOP", "TXT", "VHW", "VLW", "VPW", "VWR", "WCV", "XTE", "ZZZ"};
///< known, but not parseable
const char *sentences_known[15] = {"APB", "DPT", "GSV", "HDG", "MWD", "ROT", "RPM", "RSA", "VDR", "VTG", "ZDA", "ZZZ"};
#else  // make the lists short to save memory
///< parseable sentence ids
const char *sentences_parsed[6] = {"GGA", "GLL", "GSA", "RMC", "TOP", "ZZZ"};
///< known, but not parseable
const char *sentences_known[4] = {"DBT", "HDM", "HDT", "ZZZ"};
#endif

}  // namespace AdafruitGpsConstants

#endif