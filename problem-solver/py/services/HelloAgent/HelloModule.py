from common import ScModule, ScKeynodes, ScPythonEventType
from keynodes import Keynodes
from HelloAgent import HelloAgent

from sc import *


class HelloModule(ScModule):

    def __init__(self):
        ScModule.__init__(
            self,
            ctx=__ctx__,
            cpp_bridge=__cpp_bridge__,
            keynodes=[
            ],
            )

    def OnInitialize(self, params):
        print('Initialize Hello module')        
        kHello = self.ctx.HelperResolveSystemIdtf("hello", ScType.NodeConst)
        kWorld = self.ctx.HelperResolveSystemIdtf("world", ScType.NodeConst)
        
        agent = HelloAgent(self)
        agent.Register(kHello, ScPythonEventType.AddOutputEdge)        

        tempEdgeAddr = self.ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, kHello, kWorld)
        self.ctx.DeleteElement(tempEdgeAddr)
        self.ctx.DeleteElement(kHello)
        self.ctx.DeleteElement(kWorld)

    def OnShutdown(self):        
        print('Shutting down Hello module')  


service = HelloModule()
service.Run()
