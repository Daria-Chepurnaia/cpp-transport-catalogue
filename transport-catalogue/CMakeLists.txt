cmake_minimum_required(VERSION 3.10)

project(TransportCatalogue)

set(CMAKE_CXX_STANDARD 17)

add_executable(TransportCatalogue main.cpp
                                  domain.h                                    
                                  geo.h 
                                  geo.cpp 
                                  graph.h 
                                  graph.h 
                                  json_builder.h 
                                  json_builder.cpp 
                                  json_reader.h 
                                  json_reader.cpp 
                                  json.h 
                                  json.cpp 
                                  map_renderer.h 
                                  map_renderer.cpp 
                                  ranges.h 
                                  request_handler.h 
                                  request_handler.cpp 
                                  router.h 
                                  svg.cpp 
                                  svg.h 
                                  transport_catalogue.h 
                                  transport_catalogue.cpp 
                                  transport_router.h 
                                  transport_router.cpp)
