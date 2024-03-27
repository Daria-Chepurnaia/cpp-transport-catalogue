#include <iostream>
#include <string>

#include "request_handler.h"
#include "json_reader.h"
#include "transport_router.h"

using namespace std;
using namespace transport_catalogue;

int main() {
    TransportCatalogue catalogue;    
    
    json::Document doc = json::Load(cin);    
    JSONReader reader(doc); 
    
    reader.FillCatalogue(catalogue);

    TransportRouter transport_router(catalogue, reader.GetBusWaitTime(), reader.GetBusVelocity());    
    
    renderer::RenderSettings settings;   
    renderer::MapRenderer renderer(reader.GetRenderSettings());
    RequestHandler handler(catalogue, renderer);
    renderer.SetBusesToRender(handler.GetAllRoutesWithInfo());
    renderer.SetInfoBusesToRoundtrip(reader.GetBusNameToRoundTrip());    
    std::ostringstream out;
    handler.RenderMap(out);    
    reader.SetTransportRouter(&transport_router);
    Document doc_to_optput = reader.MakeJSON(catalogue, out);
    
    json::Print(doc_to_optput, cout);    
}