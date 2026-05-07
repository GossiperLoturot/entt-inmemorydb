#[allow(dead_code, unused_imports)]
mod simulation_state_generated;

fn main() {
    println!("start task");

    let ctx = zmq::Context::new();
    let socket = ctx.socket(zmq::REQ).unwrap();
    socket.connect("ipc://pipe.run").unwrap();

    println!("send msg");
    let send_msg = zmq::Message::new();
    socket.send(send_msg, Default::default()).unwrap();

    let mut recv_msg = zmq::Message::new();
    socket.recv(&mut recv_msg, Default::default()).unwrap();
    println!("recv msg");

    let buf = &recv_msg[..];
    let state = simulation_state_generated::root_as_simulation_state(buf).unwrap();
    println!("{:?}", state);

    println!("complete task");
}
