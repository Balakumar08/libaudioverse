#Schedules 3 copies of the same sound to happen in the future.
import libaudioverse
import time
libaudioverse.initialize()
server = libaudioverse.Server()
print("Enter the path to a sound file:")
path = input()
buffer = libaudioverse.Buffer(server)
buffer.load_from_file(path)
timeline = libaudioverse.BufferTimelineNode(server, channels = 2)
timeline.schedule_buffer(buffer = buffer, time = 0.0, pitch_bend = 1.0)
timeline.schedule_buffer(buffer, time = 1.0, pitch_bend = 0.5)
timeline.schedule_buffer(buffer = buffer, time = 2.0, pitch_bend = 2.0)
timeline.connect(0, timeline.server)
server.set_output_device()
print("Playing...")
time.sleep(30.0)
libaudioverse.shutdown()