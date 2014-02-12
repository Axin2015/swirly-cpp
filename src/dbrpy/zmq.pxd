cdef extern from "zmq.h":

    int zmq_errno()

    const char* zmq_strerror(int errnum)

    void* zmq_ctx_new()

    int zmq_ctx_destroy(void* context)
