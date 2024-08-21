<script lang="ts">
    export let label: string;
    export let value: number;
    export let min = 0;
    export let max = 1;
    export let step = 0.01;
    export let onChange: () => void;
</script>

<label class="label cursor-pointer" for={label}>
    <span class="mr-4">{label}</span>
</label>
<input 
    type="range"
    min={min} 
    max={max}
    step={step}
    class="range range-primary"
    id={label}
    bind:value={value}
    on:change={onChange}
/>