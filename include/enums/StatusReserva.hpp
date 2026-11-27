#ifndef STATUSRESERVA_HPP
#define STATUSRESERVA_HPP

// Estados de uma reserva: Confirmada -> Concluida, Cancelada ou Falta.
enum class StatusReserva {
    Confirmada,
    Concluida,
    Cancelada,
    Falta
};

#endif
