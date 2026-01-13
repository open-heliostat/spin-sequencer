import type { PageLoad } from './$types';

export const load = (async () => {
    return {
        title: 'UDP Sender'
    };
}) satisfies PageLoad;
