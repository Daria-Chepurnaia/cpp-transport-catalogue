#include <iostream>
#include <string>

#include "request_handler.h"
#include "json_reader.h"

using namespace std;
using namespace transport_catalogue;

int main() {
    TransportCatalogue catalogue;    
    
    json::Document doc = json::Load(cin);    
    JSONReader reader(doc);
    
    
    reader.FillCatalogue(catalogue);
    
    renderer::RenderSettings settings;   
    renderer::MapRenderer renderer(reader.GetRenderSettings());
    RequestHandler handler(catalogue, renderer);
    renderer.SetBusesToRender(handler.GetAllRoutesWithInfo());
    renderer.SetInfoBusesToRoundtrip(reader.GetBusNameToRoundTrip());    
    std::ostringstream out;
    handler.RenderMap(out);    
    Document doc_to_optput = reader.MakeJSON(catalogue, out);
    
    reader.PrintInfo(doc_to_optput, cout);    
}