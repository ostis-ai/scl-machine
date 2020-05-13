from sctp.client import SctpClient
from sctp.types import ScElementType


class AgentCaller:

    def __init__(self):
        self.sctp_client = SctpClient()
        self.sctp_client.initialize("localhost", 55770)
        self.question = self.sctp_client.find_element_by_system_identifier("question")
        self.question_initiated = self.sctp_client.find_element_by_system_identifier(
            "question_initiated")

    def __del__(self):
        del self.sctp_client

    def call(self, question_name_idtf, question_node_idtf):
        question_name = self.sctp_client.find_element_by_system_identifier(question_name_idtf)
        question_node = self.sctp_client.find_element_by_system_identifier(question_node_idtf)

        self.sctp_client.create_arc(ScElementType.sc_type_arc_pos_const_perm,
                                    self.question,
                                    question_node)
        self.sctp_client.create_arc(ScElementType.sc_type_arc_pos_const_perm,
                                    question_name,
                                    question_node)
        self.sctp_client.create_arc(ScElementType.sc_type_arc_pos_const_perm,
                                    self.question_initiated,
                                    question_node)


if __name__ == '__main__':
    runner = AgentCaller()
    runner.call("question_direct_inference", "question_direct_inference_test")
