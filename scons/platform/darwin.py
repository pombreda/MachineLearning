############################################################################
# LGPL License                                                             #
#                                                                          #
# This file is part of the Machine Learning Framework.                     #
# Copyright (c) 2010, Philipp Kraus, <philipp.kraus@flashpixx.de>          #
# This program is free software: you can redistribute it and/or modify     #
# it under the terms of the GNU Lesser General Public License as           #
# published by the Free Software Foundation, either version 3 of the       #
# License, or (at your option) any later version.                          #
#                                                                          #
# This program is distributed in the hope that it will be useful,          #
# but WITHOUT ANY WARRANTY; without even the implied warranty of           #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
# GNU Lesser General Public License for more details.                      #
#                                                                          #
# You should have received a copy of the GNU Lesser General Public License #
# along with this program. If not, see <http://www.gnu.org/licenses/>.     #
############################################################################

# -*- coding: utf-8 -*-

import os
Import("*")


# OSX system information
if "CXX" in os.environ:
    conf.env.Replace(CXX = os.environ["CXX"])
    print("Using compiler " + os.environ["CXX"])

if "CPPPATH" in os.environ :
    conf.env.Append(CPPPATH = os.environ["CPPPATH"].split(os.pathsep))
    print("Appending custom C++ path (CPPPATH)")

if "CXXFLAGS" in os.environ:
    conf.env.Append(CXXFLAGS = os.environ["CXXFLAGS"].split(" "))
    print("Appending custom build flags (CXXFLAGS)")
 
if "LDFLAGS" in os.environ:
    conf.env.Append(LINKFLAGS = os.environ["LDFLAGS"].split(" "))
    print("Appending custom link flags (LDFLAG)")

if "DYLD_LIBRARY_PATH" in os.environ :
    conf.env.Append(LIBPATH = os.environ["DYLD_LIBRARY_PATH"].split(os.pathsep)) 
    print("Appending custom OSX dynamic library path (DYLD_LIBRARY_PATH)")
elif "LD_LIBRARY_PATH" in os.environ :
    conf.env.Append(LIBPATH = os.environ["LD_LIBRARY_PATH"].split(os.pathsep)) 
    print("Appending custom library path (LD_LIBRARY_PATH)")
elif "LIBRARY_PATH" in os.environ :
    conf.env.Append(LIBPATH = os.environ["LIBRARY_PATH"].split(os.pathsep)) 
    print("Appending custom library path (LIBRARY_PATH)")


# main configuration
conf.env.Append(CXXFLAGS = "-fopenmp -pipe -D BOOST_FILESYSTEM_NO_DEPRECATED -D BOOST_NUMERIC_BINDINGS_BLAS_CBLAS")    
conf.env.Append(LINKFLAGS = "-pthread -fopenmp")

localconf = {
    "libraries" : [  "boost_system",
                     "boost_regex"
    ],
                  
    "librariesWithHeader" : [
            { "lib" :  "boost_iostreams", "lang" : "CXX", "header" : [ "boost/iostreams/filter/gzip.hpp", "boost/iostreams/filter/bzip2.hpp", "boost/iostreams/stream.hpp", "boost/iostreams/device/null.hpp", "boost/iostreams/filtering_streambuf.hpp", "boost/iostreams/filtering_stream.hpp", "boost/iostreams/filter/counter.hpp", "boost/iostreams/concepts.hpp", "boost/iostreams/operations.hpp", "boost/iostreams/copy.hpp"] } 
    ],
    
    "cheaders"   :  [ "omp.h" ],
    
    "cppheaders" :  [  "map",
                      "algorithm",
                      "limits",
                      "iostream",
                      "numeric",
                      "string",
                      "sstream",
                      "ostream",
                      "fstream",
                      "stdexcept",
                      "vector",
                      "ctime",
                      "iterator",
                      "typeinfo",
                      "cxxabi.h",
                
                      "boost/numeric/ublas/matrix.hpp",
                      "boost/numeric/ublas/vector.hpp", 
                      "boost/numeric/ublas/matrix_sparse.hpp",
                      "boost/numeric/ublas/vector_proxy.hpp",
                      "boost/numeric/ublas/matrix_proxy.hpp",
                      "boost/numeric/ublas/storage.hpp",
                      "boost/numeric/ublas/io.hpp",
                      "boost/numeric/ublas/symmetric.hpp", 
                
                      "boost/numeric/bindings/blas.hpp",
                      "boost/numeric/bindings/ublas/vector.hpp",
                      "boost/numeric/bindings/ublas/matrix.hpp",
                      "boost/numeric/bindings/lapack/driver/geev.hpp",
                      "boost/numeric/bindings/lapack/driver/ggev.hpp",
                      "boost/numeric/bindings/lapack/driver/gesv.hpp",
                      "boost/numeric/bindings/lapack/driver/gesvd.hpp",
                      "boost/numeric/bindings/lapack/computational/hseqr.hpp",
                
                      "boost/algorithm/string.hpp", 
                      "boost/algorithm/string/erase.hpp",
                
                      "boost/accumulators/accumulators.hpp",
                      "boost/accumulators/statistics/stats.hpp",
                      "boost/accumulators/statistics/min.hpp",
                      "boost/accumulators/statistics/max.hpp",
                      "boost/accumulators/statistics/mean.hpp",
                      "boost/accumulators/statistics/variance.hpp",
                      "boost/math/distributions/beta.hpp",
                      "boost/math/distributions/students_t.hpp",
                      "boost/math/distributions/weibull.hpp",
                      "boost/math/distributions/rayleigh.hpp",
                      "boost/math/distributions/pareto.hpp",
                      "boost/math/distributions/chi_squared.hpp",
                      "boost/math/distributions/triangular.hpp",
                
                      "boost/iterator/counting_iterator.hpp",
                      "boost/iterator/permutation_iterator.hpp",
                
                      "boost/lambda/lambda.hpp",
                
                      "boost/foreach.hpp",
                      "boost/random.hpp",
                      "boost/shared_ptr.hpp",
                      "boost/static_assert.hpp",
                      "boost/bind.hpp",
                      "boost/ref.hpp",
                      "boost/regex.hpp",
                      "boost/lexical_cast.hpp",
                   ]
}
      

if not("javac" in COMMAND_LINE_TARGETS) :
    localconf["librariesWithHeader"].extend([
        {"lib" : "boost_program_options", "lang" : "CXX", "header" : ["boost/program_options/parsers.hpp", "boost/program_options/variables_map.hpp", "boost/program_options/options_description.hpp"] },
        {"lib" : "boost_filesystem", "lang" : "CXX", "header" : "boost/filesystem.hpp" }
    ])
    localconf["libraries"].extend(["boost_exception"])
    localconf["cppheaders"].extend( ["cstdlib"] )

if conf.env["atlaslink"] == "multi" :
    localconf["libraries"].extend( ["ptcblas", "ptf77blas", "gfortran"] )
elif conf.env["atlaslink"] == "single" :
    localconf["libraries"].extend( ["cblas", "f77blas", "gfortran"] )

if conf.env["staticlink"] :
    localconf["libraries"].append("atlas")
else :
    if conf.env["atlaslink"] == "multi" :
        localconf["libraries"].append("tatlas")
    elif conf.env["atlaslink"] == "single" :
        localconf["libraries"].append("satlas")

if conf.env["withdebug"] :
    conf.env.Append(CXXFLAGS = "-g")
else :
    conf.env.Append(CXXFLAGS = "-D NDEBUG -D BOOST_UBLAS_NDEBUG")
    
if conf.env["withsources"] :
    conf.env.Append(CXXFLAGS = "-D MACHINELEARNING_SOURCES -D MACHINELEARNING_SOURCES_TWITTER")
    localconf["librariesWithHeader"].extend([
        {"lib" : "xm2", "lang" : "C", "header" : ["libxml/parser.h", "libxml/tree.h", "libxml/xpath.h", "libxml/xpathInternals.h"]},
        {"lib" : "json", "lang" : "CXX", "header" : "json/json.h"}
    ])
    localconf["cppheaders"].extend( ["locale", "boost/asio.hpp", "boost/xpressive/xpressive.hpp", "boost/date_time/time_facet.hpp", "boost/date_time/gregorian/gregorian.hpp", "boost/date_time/local_time/local_time.hpp"] )

if conf.env["withrandomdevice"] :
    conf.env.Append(CXXFLAGS = "-D MACHINELEARNING_RANDOMDEVICE")
    localconf["librariesWithHeader"].append({"lib" : "boost_random", "lang" : "CXX", "header" : "boost/nondet_random.hpp"})
    
if conf.env["withmpi"] :
    conf.env.Append(CXXFLAGS = "-D MACHINELEARNING_MPI")
    localconf["librariesWithHeader"].append({"lib" : "boost_mpi", "lang" : "CXX", "header" : "boost/mpi.hpp"})
    localconf["libraries"].append( "boost_serialization" )
    conf.env.Replace(CXX = "mpic++")
    
if conf.env["withmultilanguage"] :
    conf.env.Append(CXXFLAGS = "-D MACHINELEARNING_MULTILANGUAGE")
    localconf["librariesWithHeader"].append({"lib" : "intl", "lang" : "C", "header" : "libintl.h"})
    
if conf.env["withfiles"] :
    conf.env.Append(CXXFLAGS = "-D MACHINELEARNING_FILES -D MACHINELEARNING_FILES_HDF")
    localconf["librariesWithHeader"].append({"lib" : "hdf5_cpp", "lang" : "CXX", "header" : "H5Cpp.h"})
    localconf["libraries"].append( ["hdf5"] )
    
if conf.env["withsymbolicmath"] :
    conf.env.Append(CXXFLAGS = "-D MACHINELEARNING_SYMBOLICMATH")
    localconf["librariesWithHeader"].append({"lib" : "ginac", "lang" : "CXX", "header" : ["boost/multi_array.hpp", "boost/variant.hpp", "ginac/ginac.h"]})

if conf.env["withlogger"] :
    conf.env.Append(CXXFLAGS = "-D MACHINELEARNING_LOGGER")
    localconf["libraries"].append("boost_thread")
    
if conf.env["withoptimize"] :
    conf.env.Append(CXXFLAGS = "-O2 -fomit-frame-pointer -finline-functions")
    if conf.env["math"] == "sse3" :
        conf.env.Append(CXXFLAGS = "-mfpmath=sse -msse3")
    elif conf.env["math"] == "sse" :
        conf.env.Append(CXXFLAGS = "-mfpmath=sse -msse")
    else :
        conf.env.Append(CXXFLAGS = "-mfpmath=387")





help.checkConfiguratin( conf, localconf )








Exit(1)

# check library data (Lapack is linked always static)
#if conf.env["staticlink"] :
#    conf.env.Append(LINKFLAGS = "-static")
#    libraries = [conf.env["LIBPREFIX"]+i+conf.env["LIBSUFFIX"] for i in libraries]
#    libraries.extend( ["z", "bz2"] )
#libraries.append(conf.env["LIBPREFIX"]+"lapack"+conf.env["LIBSUFFIX"])



