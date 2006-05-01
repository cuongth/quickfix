import quickfix as fix

class Message(fix.Message):
	def __init__(self):
		fix.Message.__init__(self)
		self.getHeader().setField( fix.BeginString("FIX.4.0") )

class Heartbeat(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("0") )

class Logon(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("A") )

class TestRequest(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("1") )

class ResendRequest(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("2") )

class Reject(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("3") )

class SequenceReset(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("4") )

class Logout(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("5") )

class Advertisement(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("7") )

class IndicationofInterest(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("6") )

class News(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("B") )

	class LinesOfText(fix.Group):
		def __init__(self):
			order = fix.intArray(2)
			order[0] = 58
			order[1] = 0
			fix.Group.__init__(self, 33, 58, order)

class Email(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("C") )

	class LinesOfText(fix.Group):
		def __init__(self):
			order = fix.intArray(2)
			order[0] = 58
			order[1] = 0
			fix.Group.__init__(self, 33, 58, order)

class QuoteRequest(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("R") )

class Quote(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("S") )

class NewOrderSingle(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("D") )

class ExecutionReport(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("8") )

	class NoMiscFees(fix.Group):
		def __init__(self):
			order = fix.intArray(4)
			order[0] = 137
			order[1] = 138
			order[2] = 139
			order[3] = 0
			fix.Group.__init__(self, 136, 137, order)

class DontKnowTrade(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("Q") )

class OrderCancelReplaceRequest(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("G") )

class OrderCancelRequest(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("F") )

class OrderCancelReject(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("9") )

class OrderStatusRequest(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("H") )

class Allocation(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("J") )

	class NoOrders(fix.Group):
		def __init__(self):
			order = fix.intArray(5)
			order[0] = 11
			order[1] = 37
			order[2] = 66
			order[3] = 105
			order[4] = 0
			fix.Group.__init__(self, 73, 11, order)

	class NoExecs(fix.Group):
		def __init__(self):
			order = fix.intArray(5)
			order[0] = 17
			order[1] = 32
			order[2] = 31
			order[3] = 30
			order[4] = 0
			fix.Group.__init__(self, 124, 17, order)

	class NoMiscFees(fix.Group):
		def __init__(self):
			order = fix.intArray(4)
			order[0] = 137
			order[1] = 138
			order[2] = 139
			order[3] = 0
			fix.Group.__init__(self, 136, 137, order)

	class NoAllocs(fix.Group):
		def __init__(self):
			order = fix.intArray(9)
			order[0] = 79
			order[1] = 80
			order[2] = 81
			order[3] = 76
			order[4] = 109
			order[5] = 12
			order[6] = 13
			order[7] = 85
			order[8] = 0
			fix.Group.__init__(self, 78, 79, order)

		class NoDlvyInst(fix.Group):
			def __init__(self):
				order = fix.intArray(3)
				order[0] = 92
				order[1] = 86
				order[2] = 0
				fix.Group.__init__(self, 85, 92, order)

class AllocationACK(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("P") )

class NewOrderList(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("E") )

class ListStatus(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("N") )

	class NoOrders(fix.Group):
		def __init__(self):
			order = fix.intArray(5)
			order[0] = 11
			order[1] = 14
			order[2] = 84
			order[3] = 6
			order[4] = 0
			fix.Group.__init__(self, 73, 11, order)

class ListExecute(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("L") )

class ListCancelRequest(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("K") )

class ListStatusRequest(Message):
	def __init__(self):
		Message.__init__(self)
		self.getHeader().setField( fix.MsgType("M") )