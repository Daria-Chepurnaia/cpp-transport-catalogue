
# C++ Transport Catalogue

## Project Description

This project implements a transport catalogue system designed for managing bus stops and transportation routes, along with processing requests. The system supports adding stops and routes, as well as searching and retrieving information about them. Key functionalities include graphical output, route searching, and travel time calculations. The project is built using C++ and incorporates a directed weighted graph for routing functionalities.

This program can create similar route diagrams: [Route Diagram](transport-catalogue/map_example.png)

## Features

- **Stop Management:**  Add stops with names and geographical coordinates (latitude and longitude).
- **Bus Route Management:** Add bus routes, specifying stops and whether the route is a round trip.
- **Route Information Retrieval:** Get the number of stops, unique stops, and the total length of the route.  
- **Route Searching:** Find routes between two stops and calculate travel time.  
- **Graph Representation:** Utilize a directed weighted graph for efficient route and distance calculations.  
- **JSON Support:** Input and output data in JSON format, allowing for structured data management and easy integration.  
- **Rendering:** Generate SVG diagrams of routes with customizable rendering settings. 

## Prerequisites

Before building the project, make sure you have the following software installed on your system:

- **C++ Compiler:** A modern C++ compiler (like GCC or Clang) that supports C++17 or later.
- **CMake:** A tool for managing the build process of the project. You can download it from [CMake's official website](https://cmake.org/download/).

## How to Build and Run

### Step 1: Clone the repository:

   ```bash
    git clone git@github.com:Daria-Chepurnaia/cpp-transport-catalogue.git

   ```

### Step 2: Navigate to the project directory:

   ```bash
    cd ./transport_catalogue
   ```
### Step 3: Build the project using `CMake`:

   ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build .
   ```

### Step 4: Run the program:

   ```bash
    ./TransportCatalogue
   ```

## Usage

After launching the program, it will wait for you to provide input in the form of a JSON file. This JSON text should adhere to the specified format for defining stops and bus routes.

The database is populated using the data under the `"base_requests"` key. This key should contain all necessary information for initializing the transport catalogue, including stops, buses, and distances.

Additionally, the program processes queries from the `"stat_requests"` key. This key is used to make requests to the database after it has been populated, allowing users to retrieve information about routes, stops, and other statistics.


### Example Input Data

```json
{
  "base_requests": [
    {
      "type": "Stop",
      "name": "Main Station",
      "latitude": 48.1351,
      "longitude": 11.582,
      "road_distances": {
        "City Center": 2200,
        "Old Town": 3500
      }
    },
    {
      "type": "Stop",
      "name": "City Center",
      "latitude": 48.1371,
      "longitude": 11.5756,
      "road_distances": {
        "Main Station": 2200,
        "University": 1800
      }
    },
    {
      "type": "Stop",
      "name": "University",
      "latitude": 48.1505,
      "longitude": 11.5836,
      "road_distances": {
        "City Center": 1800,
        "Old Town": 2600
      }
    },
    {
      "type": "Bus",
      "name": "Line 25",
      "stops": ["Main Station", "City Center", "University", "Old Town"],
      "is_roundtrip": false
    }
  ],
  "render_settings":
  {
    "width": 200,
    "height": 200,
    "padding": 30,
    "stop_radius": 5,
    "line_width": 14,
    "bus_label_font_size": 20,
    "bus_label_offset": [7, 15],
    "stop_label_font_size": 20,
    "stop_label_offset": [7, -3],
    "underlayer_color": [255,255,255,0.85],
    "underlayer_width": 3,
    "color_palette": ["green", [255,160,0],"red"]
  },
  "stat_requests": [
    {
      "id": 1,
      "type": "Map"
    }
    {
      "id": 1,
      "type": "Bus",
      "name": "Line 25"
    }
  ]
}
```

### Example Output

```json
[
  {
    "request_id": 1,
    "curvature": 1.2,
    "route_length": 7600,
    "stop_count": 4,
    "unique_stop_count": 4
  }
]
```

## Technologies Used

- **C++:** Core programming language.  
- **JSON:** Data interchange format for input and output.  
- **Graphs:** Directed weighted graph structure for routing.  
- **SVG:** Scalable Vector Graphics for rendering routes and stops.  

## Conclusion

This project serves as a practical application of C++ programming, focusing on data handling and routing algorithms. It highlights the use of design patterns and efficient data structures, providing valuable experience in developing scalable applications.


## License

MIT License. See [LICENSE](https://github.com/Daria-Chepurnaia/cpp-transport-catalogue/blob/main/LICENSE.txt) for more details.

